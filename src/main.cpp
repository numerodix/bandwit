#include <chrono>
#include <csignal>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <thread>

#include "sampling/ip_cmd_sampler.hpp"
#include "sampling/procfs_sampler.hpp"
#include "sampling/sysfs_sampler.hpp"
#include "termui/bar_chart.hpp"
#include "termui/signals.hpp"
#include "termui/terminal_driver.hpp"
#include "termui/terminal_mode.hpp"
#include "termui/terminal_surface.hpp"
#include "termui/terminal_window.hpp"

namespace bmon {
namespace termui {

enum class DisplayMode {
    DISPLAY_RX,
    DISPLAY_TX,
};

std::optional<DisplayMode>
read_input(TerminalSurface &surface, std::chrono::milliseconds sleep_duration) {
    // We're reading from stdin non-blocking, so we need to combine sleeping
    // with reading input
    std::this_thread::sleep_for(sleep_duration);

    char ch = fgetc(stdin);
    while (ch >= 0) {
        if (ch == '\n') {
            surface.on_carriage_return();

        } else if (ch == 'r') {
            return std::optional<DisplayMode>(DisplayMode::DISPLAY_RX);
        } else if (ch == 't') {
            return std::optional<DisplayMode>(DisplayMode::DISPLAY_TX);
        }

        ch = fgetc(stdin);
    }

    return std::nullopt;
}

std::optional<DisplayMode> input_loop(TerminalSurface &surface,
                                      std::chrono::milliseconds time_budget) {
    // We have 1s of time to spend. We'd rather do more loops with shorter
    // sleeps for greater responsiveness
    auto num_loops = 100;
    auto sleep_duration = time_budget / num_loops;

    for (auto i = 0; i < num_loops; ++i) {
        auto new_mode = read_input(surface, sleep_duration);
        if (new_mode.has_value()) {
            return new_mode;
        }
    }

    return std::nullopt;
}

void display_bar_chart(const std::unique_ptr<sampling::Sampler> &sampler,
                       const std::string &iface_name, TerminalSurface &surface,
                       BarChart &bar_chart) {
    DisplayMode mode = DisplayMode::DISPLAY_RX;

    std::vector<uint64_t> rxs{};
    std::vector<uint64_t> txs{};

    sampling::Sample prev_sample = sampler->get_sample(iface_name);

    while (true) {
        // this used to be a sleep, but now we intermix sleeping with reading
        // input non-blocking
        auto new_mode = input_loop(surface, std::chrono::milliseconds{1000});
        if (new_mode.has_value()) {
            mode = new_mode.value();
        }

        sampling::Sample sample = sampler->get_sample(iface_name);

        auto rx = sample.rx - prev_sample.rx;
        auto tx = sample.tx - prev_sample.tx;
        rxs.push_back(rx);
        txs.push_back(tx);

        // make sure the vector isn't longer than the width of the display
        if (rxs.size() > bar_chart.get_width()) {
            rxs.erase(rxs.begin());
        }
        if (txs.size() > bar_chart.get_width()) {
            txs.erase(txs.begin());
        }

        prev_sample = sample;

        if (mode == DisplayMode::DISPLAY_RX) {
            bar_chart.draw_bars_from_right("received", rxs);
        } else {
            bar_chart.draw_bars_from_right("transmitted", txs);
        }
    }
}

void run(const std::string &iface_name) {
    std::unique_ptr<sampling::Sampler> sys_sampler{
        new sampling::SysFsSampler()};

    SignalSuspender susp_sigint{SIGINT};
    SignalSuspender susp_sigwinch{SIGWINCH};

    TerminalModeSet mode_set{};
    auto mode_setter =
        mode_set.local_off(ECHO).local_off(ICANON).build_setter(&susp_sigint);
    TerminalModeGuard mode_guard{&mode_setter};

    // To read the cursor on terminal startup stdin has to be blocking
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) & ~O_NONBLOCK);

    TerminalDriver driver{stdin, stdout};
    auto terminal_window = TerminalWindow::create(&driver, &susp_sigwinch);
    // unique_ptr to ensure deletion of terminal_window
    auto win = std::unique_ptr<TerminalWindow>(terminal_window);

    TerminalSurface surface{terminal_window, 10};
    BarChart bar_chart{&surface};

    // To read user keyboard input we need stdin to be non-blocking
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);

    display_bar_chart(sys_sampler, iface_name, surface, bar_chart);
}

} // namespace termui
} // namespace bmon

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Must pass <iface_name>\n";
        exit(EXIT_FAILURE);
    }

    std::string iface_name{argv[1]};

    // We expect to get a Ctrl+C. Install a SIGINT handler that throws an
    // exception such that we can unwind orderly and enter the catch block
    // below.
    signal(SIGINT, bmon::termui::sigint_handler);

    // We desperately need to wrap the execution in a try/catch otherwise an
    // uncaught exception will terminate the program bypassing all destructors
    // and leave the terminal in a corrupted state.
    try {
        bmon::termui::run(iface_name);
    } catch (bmon::termui::InterruptException &e) {
        // This is the expected way to stop the program. Nothing to do here.
    } catch (std::exception &e) {
        std::cerr << "Trapped uncaught exception:\n  " << e.what();
        exit(EXIT_FAILURE);
    } catch (...) {
        std::cerr << "This is the last resort exception handler. I have no "
                     "state about the error.";
        exit(EXIT_FAILURE);
    }

    return 0;
}