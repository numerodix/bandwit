#include <csignal>
#include <fcntl.h>
#include <unistd.h>

#include "sampling/sampler_detector.hpp"
#include "termui.hpp"
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

    kb_reader_ = std::make_unique<KeyboardInputReader>(stdin);

    auto now = Clock::now();
    ts_rx_ = std::make_unique<sampling::TimeSeries>(one_sec_, now);
    ts_tx_ = std::make_unique<sampling::TimeSeries>(one_sec_, now);

    prev_sample_ = sampler_->get_sample(iface_name_);
}

TermUi::~TermUi() {
    non_blocking_status_setter_->reset();
    mode_setter_->reset();
}

void TermUi::run_forever() {
    while (true) {
        // Time execution of sampling and rendering
        auto pre = Clock::now();

        sample();
        render();

        // Spend the rest of the second reading keyboard input
        auto elapsed = Clock::now() - pre;
        auto remaining = MILLIS(one_sec_ - elapsed);

        read_keyboard_input(remaining);
    }
}

void TermUi::sample() {
    sampling::Sample sample = sampler_->get_sample(iface_name_);

    auto now = Clock::from_time_t(sample.ts);
    auto rx = sample.rx - prev_sample_.rx;
    auto tx = sample.tx - prev_sample_.tx;

    ts_rx_->set(now, rx);
    ts_tx_->set(now, tx);

    prev_sample_ = sample;
}

void TermUi::render() {
    if (mode_ == DisplayMode::DISPLAY_RX) {
        auto rxs = ts_rx_->get_slice_from_end(bar_chart_->get_width());
        bar_chart_->draw_bars_from_right(iface_name_, "received", rxs);
    } else {
        auto txs = ts_tx_->get_slice_from_end(bar_chart_->get_width());
        bar_chart_->draw_bars_from_right(iface_name_, "transmitted", txs);
    }
}

void TermUi::read_keyboard_input(Millis interval) {
    KeyPress key = kb_reader_->read_nonblocking(interval);

    if (key == KeyPress::CARRIAGE_RETURN) {
        terminal_surface_->on_carriage_return();

    } else if (key == KeyPress::DISPLAY_RX) {
        mode_ = DisplayMode::DISPLAY_RX;

    } else if (key == KeyPress::DISPLAY_TX) {
        mode_ = DisplayMode::DISPLAY_TX;

    } else if (key == KeyPress::QUIT) {
        throw InterruptException();
    }
}

} // namespace termui
} // namespace bmon
