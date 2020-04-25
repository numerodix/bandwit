#include <csignal>
#include <fcntl.h>
#include <unistd.h>

#include "sampling/sampler_detector.hpp"
#include "sampling/time_series.hpp"
#include "termui.hpp"
#include "termui/keyboard_input.hpp"
#include "termui/signals.hpp"

namespace bmon {
namespace termui {

TermUi::TermUi(const std::string &iface_name)
    : iface_name_{std::move(iface_name)} {
    sampling::SamplerDetector detector{};
    sampler_ = detector.detect_sampler(iface_name);

    susp_sigint_ =
        std::make_unique<SignalSuspender>(std::initializer_list<int>{SIGINT});
    susp_sigwinch_ =
        std::make_unique<SignalSuspender>(std::initializer_list<int>{SIGWINCH});

    TerminalModeSet mode_set{};
    mode_setter_ = mode_set.local_off(ECHO).local_off(ICANON).build_setterp(
        susp_sigint_.get());
    mode_setter_->set();
    // make sure the terminal has -ECHO -ICANON for the rest of this function
    // TerminalModeGuard mode_guard{&mode_setter};

    FileStatusSet blocking_status_set{};
    // give the driver a way to make stdin blocking when needed
    blocking_status_setter_ =
        blocking_status_set.status_off(O_NONBLOCK).build_setterp(STDIN_FILENO);

    terminal_driver_ = std::make_unique<TerminalDriver>(
        stdin, stdout, blocking_status_setter_.get());
    auto terminal_windowp =
        TerminalWindow::create(terminal_driver_.get(), susp_sigwinch_.get());
    terminal_window_.reset(terminal_windowp);

    terminal_surface_ =
        std::make_unique<TerminalSurface>(terminal_window_.get(), 12);
    bar_chart_ = std::make_unique<BarChart>(terminal_surface_.get());

    FileStatusSet non_blocking_status_set{};
    non_blocking_status_setter_ = non_blocking_status_set.status_on(O_NONBLOCK)
                                      .build_setterp(STDIN_FILENO);
    non_blocking_status_setter_->set();
    // make sure stdin is non-blocking for the rest of this function
    // FileStatusGuard non_block_status_guard{&non_blocking_status_setter};
}

TermUi::~TermUi() {
    non_blocking_status_setter_->reset();
    mode_setter_->reset();
}

void TermUi::display_bar_chart() {
    KeyboardInputReader reader{stdin};
    DisplayMode mode = DisplayMode::DISPLAY_RX;

    std::chrono::seconds one_sec{1};

    auto now = Clock::now();
    sampling::TimeSeries ts_rx{one_sec, now};
    sampling::TimeSeries ts_tx{one_sec, now};

    sampling::Sample prev_sample = sampler_->get_sample(iface_name_);

    while (true) {
        // Time execution of sampling and rendering
        auto pre = Clock::now();

        sampling::Sample sample = sampler_->get_sample(iface_name_);

        auto rx = sample.rx - prev_sample.rx;
        auto tx = sample.tx - prev_sample.tx;

        auto now = Clock::now();
        ts_rx.set(now, rx);
        ts_tx.set(now, tx);

        prev_sample = sample;

        if (mode == DisplayMode::DISPLAY_RX) {
            auto rxs = ts_rx.get_slice_from_end(bar_chart_->get_width());
            bar_chart_->draw_bars_from_right(iface_name_, "received", rxs);
        } else {
            auto txs = ts_tx.get_slice_from_end(bar_chart_->get_width());
            bar_chart_->draw_bars_from_right(iface_name_, "transmitted", txs);
        }

        // Spend the rest of the second reading keyboard input
        auto elapsed = Clock::now() - pre;
        auto remaining = MILLIS(one_sec - elapsed);

        KeyPress key = reader.read_nonblocking(remaining);
        if (key == KeyPress::CARRIAGE_RETURN) {
            terminal_surface_->on_carriage_return();
        } else if (key == KeyPress::DISPLAY_RX) {
            mode = DisplayMode::DISPLAY_RX;
        } else if (key == KeyPress::DISPLAY_TX) {
            mode = DisplayMode::DISPLAY_TX;
        } else if (key == KeyPress::QUIT) {
            throw InterruptException();
        }
    }
}

} // namespace termui
} // namespace bmon