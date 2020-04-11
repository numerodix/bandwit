#include <chrono>
#include <csignal>
#include <iostream>
#include <map>
#include <memory>
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

using namespace bmon;

struct FlowRecord {
    uint64_t rx;
    uint64_t tx;
};

void visualize(const std::unique_ptr<sampling::Sampler> &sampler,
               const std::string &iface_name, termui::BarChart &bar_chart) {
    std::vector<uint64_t> rxs{};

    sampling::Sample prev_sample = sampler->get_sample(iface_name);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        sampling::Sample sample = sampler->get_sample(iface_name);

        FlowRecord rec{
            sample.rx - prev_sample.rx,
            sample.tx - prev_sample.tx,
        };
        rxs.push_back(rec.rx);

        // make sure the vector isn't longer than the width of the display
        if (rxs.size() > bar_chart.get_width()) {
            rxs.erase(rxs.begin());
        }

        prev_sample = sample;

        bar_chart.draw_bars_from_right(rxs);
    }
}

void run(const std::string &iface_name) {
    std::unique_ptr<sampling::Sampler> sys_sampler{
        new sampling::SysFsSampler()};

    termui::SignalSuspender susp_sigint{SIGINT};
    termui::SignalSuspender susp_sigwinch{SIGWINCH};

    termui::TerminalModeSet mode_set{};
    auto mode_setter =
        mode_set.local_off(ECHO).local_off(ICANON).build_setter(&susp_sigint);
    termui::TerminalModeGuard mode_guard{&mode_setter};

    termui::TerminalDriver driver{stdin, stdout};
    auto terminal_window =
        termui::TerminalWindow::create(&driver, &susp_sigwinch);
    // unique_ptr to ensure deletion of terminal_window
    auto win = std::unique_ptr<termui::TerminalWindow>(terminal_window);

    termui::TerminalSurface surface{terminal_window, 10};
    termui::BarChart bar_chart{&surface};

    visualize(sys_sampler, iface_name, bar_chart);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Must pass <iface_name>\n";
        exit(EXIT_FAILURE);
    }

    std::string iface_name{argv[1]};

    // We expect to get a Ctrl+C. Install a SIGINT handler that throws an
    // exception such that we can unwind orderly and enter the catch block
    // below.
    signal(SIGINT, termui::sigint_handler);

    // We desperately need to wrap the execution in a try/catch otherwise an
    // uncaught exception will terminate the program bypassing all destructors
    // and leave the terminal in a corrupted state.
    try {
        run(iface_name);
    } catch (termui::InterruptException &e) {
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