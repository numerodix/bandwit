#ifndef SYSFS_SAMPLER_H
#define SYSFS_SAMPLER_H

#include <string>
#include <unistd.h>
#include <unordered_map>

#include "sampling/sampler.hpp"

namespace bandwit {
namespace sampling {

class SysFsParser {
  public:
    enum class Quantity {
        RX_BYTES,
        TX_BYTES,
    };

    uint64_t read_file_as_number(const std::string &filepath) const;
    std::string create_filepath(const std::string &iface_name,
                                const Quantity &qtty) const;
    uint64_t read(const std::string &iface_name, const Quantity &qtty) const;

  private:
    std::unordered_map<Quantity, std::string> quantity_filenames_{
        {Quantity::RX_BYTES, "rx_bytes"},
        {Quantity::TX_BYTES, "tx_bytes"},
    };
};

class SysFsSampler : public Sampler {
  public:
    SysFsSampler() = default;
    ~SysFsSampler() override = default;

    CLASS_DISABLE_COPIES(SysFsSampler)
    CLASS_DISABLE_MOVES(SysFsSampler)

    Sample get_sample(const std::string &iface_name) const override;

  private:
    SysFsParser parser_{};
};

} // namespace sampling
} // namespace bandwit

#endif // SYSFS_SAMPLER_H