#include <csignal>
#include <fcntl.h>
#include <unistd.h>

#include "sampling/sampler_detector.hpp"
#include "termui.hpp"
#include "termui/signals.hpp"
#include "termui/terminal_window.hpp"

namespace bandwit {
namespace termui {

TermUi::TermUi(const std::string &iface_name) : iface_name_{iface_name} {
    sampling::SamplerDetector detector{};
    auto det_result = detector.detect_sampler(iface_name);

    sampler_ = std::move(det_result.sampler);
    prev_sample_ = det_result.sample;

    susp_sigint_ =
        std::make_unique<SignalSuspender>(std::initializer_list<int>{SIGINT});
    susp_sigwinch_ =
        std::make_unique<SignalSuspender>(std::initializer_list<int>{SIGWINCH});

    TerminalModeSet mode_set{};
    interactive_mode_setter_ =
        mode_set.local_off(ECHO).local_off(ICANON).build_setter(
            susp_sigint_.get());

    // make sure the terminal has -ECHO -ICANON from now on and for the lifetime
    // of TermUi
    interactive_mode_setter_->set();

    FileStatusSet blocking_status_set{};
    // give the driver a way to make stdin blocking when needed
    blocking_status_setter_ =
        blocking_status_set.status_off(O_NONBLOCK).build_setter(STDIN_FILENO);

    terminal_driver_ = std::make_unique<TerminalDriver>(
        stdin, stdout, blocking_status_setter_.get());
    TerminalWindow *terminal_window =
        TerminalWindow::create(terminal_driver_.get(), susp_sigwinch_.get());

    terminal_surface_ = std::make_unique<TerminalSurface>(terminal_window, 12);
    bar_chart_ = std::make_unique<BarChart>(terminal_surface_.get());

    FileStatusSet non_blocking_status_set{};
    non_blocking_status_setter_ = non_blocking_status_set.status_on(O_NONBLOCK)
                                      .build_setter(STDIN_FILENO);

    // make sure stdin is non-blocking from now on and for the lifetime of
    // TermUi
    non_blocking_status_setter_->set();

    kb_reader_ = std::make_unique<KeyboardInputReader>(stdin);

    auto now = Clock::now();
    const std::vector<AggregationWindow> windows{
        AggregationWindow::ONE_SECOND,
        AggregationWindow::ONE_MINUTE,
        AggregationWindow::ONE_HOUR,
        AggregationWindow::ONE_DAY,
    };

    ts_coll_rx_ = std::make_unique<TimeSeriesCollection>(now, windows);
    ts_coll_tx_ = std::make_unique<TimeSeriesCollection>(now, windows);

    // tell the surface to notify us just after it's redrawn itself
    // following a window resize
    terminal_surface_->register_resize_receiver(this);
}

TermUi::~TermUi() {
    // return stdio to the mode it was before we started
    non_blocking_status_setter_->reset();

    // return the terminal to the mode it was before we started
    interactive_mode_setter_->reset();
}

void TermUi::on_window_resize([[maybe_unused]] const Dimensions &win_dim_old,
                              [[maybe_unused]] const Dimensions &win_dim_new) {
    render();
}

void TermUi::run_forever() {
    std::chrono::seconds one_sec{1};

    while (true) {
        // Time execution of sampling and rendering
        auto pre = Clock::now();

        sample();
        render_no_winch();

        // Spend the rest of the second reading keyboard input
        auto elapsed = Clock::now() - pre;
        auto remaining = MILLIS(one_sec - elapsed);

        read_keyboard_input(remaining);
    }
}

void TermUi::sample() {
    sampling::Sample sample = sampler_->get_sample(iface_name_);

    auto tp = Clock::from_time_t(sample.ts);
    auto rx = sample.rx - prev_sample_.rx;
    auto tx = sample.tx - prev_sample_.tx;

    ts_coll_rx_->inc(tp, rx);
    ts_coll_tx_->inc(tp, tx);

    prev_sample_ = sample;
}

void TermUi::render() {
    // When we are called from `on_window_resize` the SIGWINCH signal guard is
    // already in effect, so there is no need to use it here.
    // Other callers of this function should suspend SIGWINCH before calling it.

    rescue_scroll_cursor();

    TimePoint cursor{};
    if (scroll_cursor_.has_value()) {
        cursor = scroll_cursor_.value();
    } else {
        cursor = ts_coll_rx_->max(agg_window_);
    }

    TimeSeriesSlice slice{};
    auto width = bar_chart_->get_width();
    std::string action{};

    if (display_mode_ == DisplayMode::DISPLAY_RX) {
        action = "received";
        slice = ts_coll_rx_->get_slice_from_point(agg_window_, cursor, width,
                                                  stat_mode_);
    } else {
        action = "transmitted";
        slice = ts_coll_tx_->get_slice_from_point(agg_window_, cursor, width,
                                                  stat_mode_);
    }

    bar_chart_->draw_bars_from_right(iface_name_, action, slice, display_scale_,
                                     stat_mode_);
}

void TermUi::read_keyboard_input(Millis interval) {
    KeyPress key = kb_reader_->read_nonblocking(interval);

    if (key == KeyPress::CARRIAGE_RETURN) {
        // ignore SIGWINCH while we're acting on a resize
        SignalGuard guard{susp_sigwinch_.get()};

        terminal_surface_->on_carriage_return();

    } else if (key == KeyPress::LETTER_R) {
        display_mode_ = DisplayMode::DISPLAY_RX;

    } else if (key == KeyPress::LETTER_T) {
        display_mode_ = DisplayMode::DISPLAY_TX;

    } else if (key == KeyPress::LETTER_C) {
        display_scale_ = next_scale(display_scale_);

    } else if (key == KeyPress::LETTER_S) {
        if (stat_mode_ == Statistic::AVERAGE) {
            stat_mode_ = Statistic::SUM;
        } else if (stat_mode_ == Statistic::SUM) {
            stat_mode_ = Statistic::AVERAGE;
        }

    } else if (key == KeyPress::ARROW_UP) {
        agg_window_ = sampling::next_interval(agg_window_);

    } else if (key == KeyPress::ARROW_DOWN) {
        agg_window_ = sampling::prev_interval(agg_window_);

    } else if (key == KeyPress::ARROW_LEFT) {
        scroll_left();

    } else if (key == KeyPress::ARROW_RIGHT) {
        scroll_right();

    } else if (key == KeyPress::QUIT) {
        throw InterruptException();
    }
}

void TermUi::render_no_winch() {
    // ignore SIGWINCH while we're rendering
    SignalGuard guard{susp_sigwinch_.get()};

    render();
}

bool TermUi::scroll_left() {
    bool cursor_moved = false;

    TimePoint cursor{};
    if (scroll_cursor_.has_value()) {
        cursor = scroll_cursor_.value();
    } else {
        cursor = ts_coll_rx_->max(agg_window_);
    }

    auto opt_tp = ts_coll_rx_->minus_one(agg_window_, cursor);
    if (opt_tp.has_value()) {
        scroll_cursor_.swap(opt_tp);
        cursor_moved = true;
    }

    return cursor_moved;
}

bool TermUi::scroll_right() {
    bool cursor_moved = false;

    TimePoint cursor{};
    if (scroll_cursor_.has_value()) {
        cursor = scroll_cursor_.value();
    } else {
        cursor = ts_coll_rx_->max(agg_window_);
    }

    auto opt_tp = ts_coll_rx_->plus_one(agg_window_, cursor);
    if (opt_tp.has_value()) {
        scroll_cursor_.swap(opt_tp);
        cursor_moved = true;
    } else {
        scroll_cursor_.reset();
    }

    return cursor_moved;
}

bool TermUi::rescue_scroll_cursor() {
    // If the scroll cursor gets out of bounds we need to rescue it by bringing
    // it within bounds again. This will happen when we are in scrolling mode
    // and the scrolling cursor falls off the left edge of the time series
    // during a truncate.
    bool cursor_moved = false;

    if (scroll_cursor_.has_value()) {
        auto cursor = scroll_cursor_.value();

        auto min = ts_coll_rx_->min(agg_window_);
        auto max = ts_coll_rx_->max(agg_window_);

        if (cursor < min) {
            scroll_cursor_.emplace(min);
            cursor_moved = true;
        } else if (cursor > max) {
            scroll_cursor_.emplace(max);
            cursor_moved = true;
        }
    }

    return cursor_moved;
}

} // namespace termui
} // namespace bandwit
