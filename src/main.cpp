#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <thread>

#include "sampling/ip_cmd_sampler.h"
#include "sampling/sysfs_sampler.h"

struct FlowRecord {
    uint64_t rx;
    uint64_t tx;
};

void collect(const std::unique_ptr<Sampler> &sampler,
             const std::string &iface_name) {
    std::map<std::time_t, FlowRecord> records{};

    Sample prev_sample = sampler->get_sample(iface_name);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        Sample sample = sampler->get_sample(iface_name);

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

int main() {
    std::unique_ptr<Sampler> ip_sampler{new IpCommandSampler()};
    Sample sample1 = ip_sampler->get_sample("wlp4s0");

    std::cout << "ip command sampler:\n";
    std::cout << "RX: " << sample1.rx << "\n";
    std::cout << "TX: " << sample1.tx << "\n";
    std::cout << "ts: " << sample1.ts << "\n";

    std::unique_ptr<Sampler> sys_sampler{new SysFsSampler()};
    Sample sample2 = sys_sampler->get_sample("wlp4s0");

    std::cout << "\nsysfs command sampler:\n";
    std::cout << "RX: " << sample2.rx << "\n";
    std::cout << "TX: " << sample2.tx << "\n";
    std::cout << "ts: " << sample2.ts << "\n";

    std::cout << "\nrun_forever():\n";
    collect(sys_sampler, "wlp4s0");
}