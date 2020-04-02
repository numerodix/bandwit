#include <iostream>
#include <memory>

#include <ip_cmd_sampler.h>
#include <sysfs_sampler.h>


int main() {
    std::unique_ptr<Sampler> ip_sampler{new IpCommandSampler()};
    Sample sample1 = ip_sampler->get_sample("wlp4s0");

    std::cout << "ip command sampler:" << "\n";
    std::cout << "RX: " << sample1.rx << "\n";
    std::cout << "TX: " << sample1.tx << "\n";
    std::cout << "ts: " << sample1.ts << "\n";


    std::unique_ptr<Sampler> sys_sampler{new SysFsSampler()};
    Sample sample2 = sys_sampler->get_sample("wlp4s0");

    std::cout << "\nsysfs command sampler:" << "\n";
    std::cout << "RX: " << sample2.rx << "\n";
    std::cout << "TX: " << sample2.tx << "\n";
    std::cout << "ts: " << sample2.ts << "\n";
}