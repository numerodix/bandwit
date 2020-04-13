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

    try {
        sysfs_sampler.get()->get_sample(iface_name);
        return std::move(sysfs_sampler);
    } catch (std::runtime_error &e) {
    }

    try {
        procfs_sampler.get()->get_sample(iface_name);
        return std::move(procfs_sampler);
    } catch (std::runtime_error &e) {
    }

    try {
        ip_cmd_sampler.get()->get_sample(iface_name);
        return std::move(ip_cmd_sampler);
    } catch (std::runtime_error &e) {
    }

    try {
        netstat_cmd_sampler.get()->get_sample(iface_name);
        return std::move(netstat_cmd_sampler);
    } catch (std::runtime_error &e) {
    }

    throw std::runtime_error(
        "Could not find a sampler supported by the system for the given interface");
}

} // namespace sampling
} // namespace bmon