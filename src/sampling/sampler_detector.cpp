#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "sampler_detector.hpp"
#include "sampling/ip_cmd_sampler.hpp"
#include "sampling/netstat_cmd_sampler.hpp"
#include "sampling/procfs_sampler.hpp"
#include "sampling/sysfs_sampler.hpp"

namespace bandwit {
namespace sampling {

#define PAIR(ClsName) std::make_pair("" #ClsName, std::make_unique<ClsName>())

std::unique_ptr<Sampler>
SamplerDetector::detect_sampler(const std::string &iface_name) const {
    using Pair = std::pair<std::string, std::unique_ptr<Sampler>>;

    std::vector<Pair> samplers{};
    samplers.emplace_back(PAIR(SysFsSampler));
    samplers.emplace_back(PAIR(ProcFsSampler));
    samplers.emplace_back(PAIR(IpCommandSampler));
    samplers.emplace_back(PAIR(NetstatCommandSampler));

    std::vector<std::string> errors{};

    for (auto &pair : samplers) {
        auto &cls_name = pair.first;
        auto &sampler = pair.second;

        try {
            sampler->get_sample(iface_name);
            return std::move(sampler);

        } catch (std::runtime_error &exc) {
            std::stringstream ss{};
            ss << cls_name << ": " << std::string{exc.what()};
            errors.push_back(ss.str());
        }
    }

    // We can't actually distinguish between samplers that fail because the
    // system does not support them and samplers that fail because there is no
    // such interface, so to aid troubleshooting we echo the error from every
    // sampler
    std::cerr << "Could not find a sampler supported by the system for the "
                 "interface: "
              << iface_name << "\n";
    for (const auto &msg : errors) {
        std::cerr << "- " << msg << "\n";
    }

    throw std::runtime_error("Cannot run without a sampler");
}

} // namespace sampling
} // namespace bandwit