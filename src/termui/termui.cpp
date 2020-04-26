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

    sampler_.reset(det_result.sampler);
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

    ts_rx_ = std::make_unique<sampling::TimeSeries>(one_sec_, now);
    ts_tx_ = std::make_unique<sampling::TimeSeries>(one_sec_, now);

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
    while (true) {
        // Time execution of sampling and rendering
        auto pre = Clock::now();

        sample();

        {
            // ignore SIGWINCH while we're rendering
            SignalGuard guard{susp_sigwinch_.get()};

            render();
        }

        // Spend the rest of the second reading keyboard input
        auto elapsed = Clock::now() - pre;
        auto remaining = MILLIS(one_sec_ - elapsed);

        read_keyboard_input(remaining);
    }
}

void TermUi::sample() {
    sampling::Sample sample = sampler_->get_sample(iface_name_);

    auto tp = Clock::from_time_t(sample.ts);
    auto rx = sample.rx - prev_sample_.rx;
    auto tx = sample.tx - prev_sample_.tx;

    ts_rx_->set(tp, rx);
    ts_tx_->set(tp, tx);

    prev_sample_ = sample;
}

void TermUi::render() {
    // when we are called from `on_window_resize` the SIGWINCH signal guard is
    // already in effect, so there is no need to use it here

    if (mode_ == DisplayMode::DISPLAY_RX) {
        auto ts_agg = ts_rx_->get_aggregated(agg_interval_);
        auto slice = ts_agg.get_slice_from_end(bar_chart_->get_width());
        bar_chart_->draw_bars_from_right(iface_name_, "received", slice);

    } else {
        auto ts_agg = ts_tx_->get_aggregated(agg_interval_);
        auto slice = ts_agg.get_slice_from_end(bar_chart_->get_width());
        bar_chart_->draw_bars_from_right(iface_name_, "transmitted", slice);
    }
}

void TermUi::read_keyboard_input(Millis interval) {
    KeyPress key = kb_reader_->read_nonblocking(interval);

    if (key == KeyPress::CARRIAGE_RETURN) {
        // ignore SIGWINCH while we're acting on a resize
        SignalGuard guard{susp_sigwinch_.get()};

        terminal_surface_->on_carriage_return();

    } else if (key == KeyPress::DISPLAY_RX) {
        mode_ = DisplayMode::DISPLAY_RX;

    } else if (key == KeyPress::DISPLAY_TX) {
        mode_ = DisplayMode::DISPLAY_TX;

    } else if (key == KeyPress::CYCLE_AGG_INTERVAL) {
        agg_interval_ = sampling::next_interval(agg_interval_);

    } else if (key == KeyPress::QUIT) {
        throw InterruptException();
    }
}

} // namespace termui
} // namespace bandwit
