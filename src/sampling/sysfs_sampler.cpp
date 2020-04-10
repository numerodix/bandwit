#include <chrono>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

#include "sysfs_sampler.hpp"

namespace bmon {
namespace sampling {

class SysFsParser {

  public:
    enum class Quantity {
        RX_BYTES,
        TX_BYTES,
    };

    uint64_t read_file_as_number(const std::string &filepath) const {
        std::ifstream fl{filepath};
        if (!fl) {
            throw std::runtime_error("failed to open file for reading");
        }

        std::string contents{};
        getline(fl, contents);

        return std::stoul(contents);
    }

    std::string create_filepath(const std::string &iface_name,
                                const Quantity &qtty) const {
        std::stringstream ss{};
        std::string filename = quantity_filenames_.at(qtty);
        ss << "/sys/class/net/" << iface_name << "/statistics/" << filename;
        return ss.str();
    }

    uint64_t read(const std::string &iface_name, const Quantity &qtty) const {
        std::string filepath = create_filepath(iface_name, qtty);
        return read_file_as_number(filepath);
    }

  private:
    std::unordered_map<Quantity, std::string> quantity_filenames_{
        {Quantity::RX_BYTES, "rx_bytes"},
        {Quantity::TX_BYTES, "tx_bytes"},
    };
};

Sample SysFsSampler::get_sample(const std::string &iface_name) const {
    SysFsParser parser{};

    auto tp = std::chrono::system_clock::now();
    std::time_t ts = std::chrono::system_clock::to_time_t(tp);

    uint64_t rx = parser.read(iface_name, SysFsParser::Quantity::RX_BYTES);
    uint64_t tx = parser.read(iface_name, SysFsParser::Quantity::TX_BYTES);

    Sample sample{
        rx,
        tx,
        ts,
    };

    return sample;
}

} // namespace sampling
} // namespace bmon