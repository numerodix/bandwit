#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <thread>

#include "sampling/ip_cmd_sampler.h"
#include "sampling/procfs_sampler.h"
#include "sampling/sysfs_sampler.h"
#include "termui/bar_chart.h"
#include "termui/display.h"

using namespace bmon;

struct FlowRecord {
    uint64_t rx;
    uint64_t tx;
};

void collect(const std::unique_ptr<sampling::Sampler> &sampler,
             const std::string &iface_name) {
    std::map<std::time_t, FlowRecord> records{};

    sampling::Sample prev_sample = sampler->get_sample(iface_name);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        sampling::Sample sample = sampler->get_sample(iface_name);

        FlowRecord rec{
            sample.rx - prev_sample.rx,
            sample.tx - prev_sample.tx,
        };
        records[sample.ts] = rec;

        prev_sample = sample;

        std::cout << "ts=" << sample.ts << ": rx=" << rec.rx
                  << "B tx=" << rec.tx << "B\n";
    }
}

void visualize(const std::unique_ptr<sampling::Sampler> &sampler,
               const std::string &iface_name, termui::BarChart *bar_chart) {
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

        prev_sample = sample;

        bar_chart->draw_bars_from_right(rxs);
    }
}

int main_z(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Must pass <iface_name>\n";
        exit(EXIT_FAILURE);
    }

    std::string iface_name{argv[1]};

    std::unique_ptr<sampling::Sampler> ip_sampler{
        new sampling::IpCommandSampler()};
    sampling::Sample sample1 = ip_sampler->get_sample(iface_name);

    std::cout << "ip command sampler (" << iface_name << "):\n";
    std::cout << "RX: " << sample1.rx << "\n";
    std::cout << "TX: " << sample1.tx << "\n";
    std::cout << "ts: " << sample1.ts << "\n";

    std::unique_ptr<sampling::Sampler> proc_sampler{
        new sampling::ProcFsSampler()};
    sampling::Sample sample2 = proc_sampler->get_sample(iface_name);

    std::cout << "\nprocfs command sampler (" << iface_name << "):\n";
    std::cout << "RX: " << sample2.rx << "\n";
    std::cout << "TX: " << sample2.tx << "\n";
    std::cout << "ts: " << sample2.ts << "\n";

    std::unique_ptr<sampling::Sampler> sys_sampler{
        new sampling::SysFsSampler()};
    sampling::Sample sample3 = sys_sampler->get_sample(iface_name);

    std::cout << "\nsysfs command sampler (" << iface_name << "):\n";
    std::cout << "RX: " << sample3.rx << "\n";
    std::cout << "TX: " << sample3.tx << "\n";
    std::cout << "ts: " << sample3.ts << "\n";

    std::cout << "\nrun_forever(" << iface_name << "):\n";
    collect(sys_sampler, iface_name);
}

int main2() {
    termui::Display disp{10};
    disp.initialize();

    termui::BarChart chart{&disp};
    std::vector<uint64_t> values = {1, 2, 3};
    chart.draw_bars_from_right(values);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Must pass <iface_name>\n";
        exit(EXIT_FAILURE);
    }

    std::string iface_name{argv[1]};

    std::unique_ptr<sampling::Sampler> sys_sampler{
        new sampling::SysFsSampler()};
    sampling::Sample sample3 = sys_sampler->get_sample(iface_name);

    termui::Display disp{10};
    disp.initialize();

    termui::BarChart chart{&disp};

    visualize(sys_sampler, iface_name, &chart);
}