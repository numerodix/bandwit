#include <chrono>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

#include "aliases.hpp"
#include "except.hpp"
#include "sysfs_sampler.hpp"

namespace bandwit {
namespace sampling {

uint64_t SysFsParser::read_file_as_number(const std::string &filepath) const {
    std::ifstream fl{filepath};
    if (!fl) {
        THROW_ARGS(std::runtime_error, "failed to open file for reading: %s",
                   filepath.c_str());
    }

    std::string contents{};
    getline(fl, contents);

    return std::stoul(contents);
}

std::string SysFsParser::create_filepath(const std::string &iface_name,
                                         const Quantity &qtty) const {
    std::stringstream ss{};
    std::string filename = quantity_filenames_.at(qtty);
    ss << "/sys/class/net/" << iface_name << "/statistics/" << filename;
    return ss.str();
}

uint64_t SysFsParser::read(const std::string &iface_name,
                           const Quantity &qtty) const {
    std::string filepath = create_filepath(iface_name, qtty);
    return read_file_as_number(filepath);
}

Sample SysFsSampler::get_sample(const std::string &iface_name) const {
    auto tp = Clock::now();
    std::time_t ts = Clock::to_time_t(tp);

    uint64_t rx = parser_.read(iface_name, SysFsParser::Quantity::RX_BYTES);
    uint64_t tx = parser_.read(iface_name, SysFsParser::Quantity::TX_BYTES);

    Sample sample{
        rx,
        tx,
        ts,
    };

    return sample;
}

} // namespace sampling
} // namespace bandwit