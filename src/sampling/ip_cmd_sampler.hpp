#ifndef IP_CMD_SAMPLER_H
#define IP_CMD_SAMPLER_H

#include "sampling/sampler.hpp"

namespace bandwit {
namespace sampling {

class IpCommandSampler : public Sampler {

  public:
    IpCommandSampler() = default;
    ~IpCommandSampler() override = default;

    CLASS_DISABLE_COPIES(IpCommandSampler)
    CLASS_DISABLE_MOVES(IpCommandSampler)

    Sample get_sample(const std::string &iface_name) const override;
};

} // namespace sampling
} // namespace bandwit

#endif // IP_CMD_SAMPLER_H