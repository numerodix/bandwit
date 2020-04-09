#include <chrono>
#include <fcntl.h>
#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <signal.h>
#include <stdio.h>
#include <thread>

#include "sampling/ip_cmd_sampler.h"
#include "sampling/procfs_sampler.h"
#include "sampling/sysfs_sampler.h"
#include "termui/bar_chart.h"
#include "termui/signals.h"
#include "termui/terminal_driver.h"
#include "termui/terminal_mode.h"
#include "termui/terminal_surface.h"
#include "termui/terminal_window.h"

using namespace bmon;

struct FlowRecord {
    uint64_t rx;
    uint64_t tx;
};

void accept_input(termui::TerminalSurface &surface) {
    std::this_thread::sleep_for(std::chrono::milliseconds{10});

    char ch = fgetc(stdin);
    while (ch >= 0) {
        if (ch == 10) {
            surface.on_carriage_return();
        }

        ch = fgetc(stdin);
    }
}

void accept_loop(termui::TerminalSurface &surface) {
    for (int i = 0; i < 100; ++i) {
        accept_input(surface);
    }
}

void visualize(const std::unique_ptr<sampling::Sampler> &sampler,
               const std::string &iface_name, termui::TerminalSurface &surface,
               termui::BarChart &bar_chart) {
    std::vector<uint64_t> rxs{};

    sampling::Sample prev_sample = sampler->get_sample(iface_name);

    while (true) {
        // std::this_thread::sleep_for(std::chrono::seconds{1});
        accept_loop(surface);
        sampling::Sample sample = sampler->get_sample(iface_name);

        FlowRecord rec{
            sample.rx - prev_sample.rx,
            sample.tx - prev_sample.tx,
        };
        rxs.push_back(rec.rx);

        // make sure the vector isn't longer than the width of the display
        if (rxs.size() > surface.get_size().width) {
            rxs.erase(rxs.begin());
        }

        prev_sample = sample;

        bar_chart.draw_bars_from_right(rxs);
    }
}

void run(const std::string &iface_name) {
    std::unique_ptr<sampling::Sampler> sys_sampler{
        new sampling::SysFsSampler()};

    termui::SignalSuspender susp{SIGINT};

    termui::TerminalModeSet set{};
    auto ms = set.local_off(ECHO).local_off(ICANON).build_setter(&susp);
    termui::TerminalModeGuard mg{&ms};

    termui::TerminalDriver dr{stdin, stdout};
    auto pwin = termui::TerminalWindow::create(&dr);
    auto win = std::unique_ptr<termui::TerminalWindow>(pwin);

    termui::TerminalSurface surf{pwin, 10};
    termui::BarChart chart{&surf};

    // Modify stdin to be non-blocking
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);

    visualize(sys_sampler, iface_name, surf, chart);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Must pass <iface_name>\n";
        exit(EXIT_FAILURE);
    }

    std::string iface_name{argv[1]};

    // We desperately need to wrap the execution in a try/catch otherwise an
    // uncaught exception will terminate the program bypassing all destructors
    // and leave the terminal in a corrupted state.
    try {
        run(iface_name);
    } catch (std::exception &e) {
        std::cerr << "Trapped uncaught exception:\n  " << e.what() << "\n";
        exit(EXIT_FAILURE);
    } catch (...) {
        std::cerr << "This is the last resort exception handler. I have no "
                     "state about the error.\n";
        exit(EXIT_FAILURE);
    }
}