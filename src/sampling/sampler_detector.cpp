#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "sampler_detector.hpp"
#include "sampling/ip_cmd_sampler.hpp"
#include "sampling/netstat_cmd_sampler.hpp"
#include "sampling/procfs_sampler.hpp"
#include "sampling/sysfs_sampler.hpp"

namespace bmon {
namespace sampling {

std::unique_ptr<Sampler>
SamplerDetector::detect_sampler(const std::string &iface_name) const {
    std::unique_ptr<Sampler> sysfs_sampler = std::make_unique<SysFsSampler>();
    std::unique_ptr<Sampler> procfs_sampler = std::make_unique<ProcFsSampler>();
    std::unique_ptr<Sampler> ip_cmd_sampler =
        std::make_unique<IpCommandSampler>();
    std::unique_ptr<Sampler> netstat_cmd_sampler =
        std::make_unique<NetstatCommandSampler>();

    std::vector<std::string> errors{};

    try {
        sysfs_sampler.get()->get_sample(iface_name);
        return std::move(sysfs_sampler);
    } catch (std::runtime_error &exc) {
        std::stringstream ss{};
        ss << "SysFsSampler: " << std::string{exc.what()};
        errors.push_back(ss.str());
    }

    try {
        procfs_sampler.get()->get_sample(iface_name);
        return std::move(procfs_sampler);
    } catch (std::runtime_error &exc) {
        std::stringstream ss{};
        ss << "ProcFsSampler: " << std::string{exc.what()};
        errors.push_back(ss.str());
    }

    try {
        ip_cmd_sampler.get()->get_sample(iface_name);
        return std::move(ip_cmd_sampler);
    } catch (std::runtime_error &exc) {
        std::stringstream ss{};
        ss << "IpCommandSampler: " << std::string{exc.what()};
        errors.push_back(ss.str());
    }

    try {
        netstat_cmd_sampler.get()->get_sample(iface_name);
        return std::move(netstat_cmd_sampler);
    } catch (std::runtime_error &exc) {
        std::stringstream ss{};
        ss << "NetstatCommandSampler: " << std::string{exc.what()};
        errors.push_back(ss.str());
    }

    std::cerr << "Could not find a sampler supported by the system for the "
                 "given interface:\n";
    for (auto msg : errors) {
        std::cerr << "- " << msg << "\n";
    }

    throw std::runtime_error("Cannot run without a sampler");
}

} // namespace sampling
} // namespace bmon