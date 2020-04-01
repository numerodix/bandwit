#include <iostream>

#include <ip_cmd_sampler.h>


int main() {
    IpCommandSampler sampler{};

    Sample sample = sampler.get_sample("wlp4s0");

    std::cout << "RX: " << sample.rx << "\n";
    std::cout << "TX: " << sample.tx << "\n";
    std::cout << "ts: " << sample.ts << "\n";
}