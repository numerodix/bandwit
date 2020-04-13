#ifndef NETSTAT_CMD_SAMPLER_H
#define NETSTAT_CMD_SAMPLER_H

#include "sampling/sampler.hpp"

namespace bmon {
namespace sampling {

// This is the only method that is known to work on BSD
class NetstatCommandSampler : public Sampler {

  public:
    NetstatCommandSampler() = default;
    ~NetstatCommandSampler() override = default;

    CLASS_DISABLE_COPIES(NetstatCommandSampler)
    CLASS_DISABLE_MOVES(NetstatCommandSampler)

    Sample get_sample(const std::string &iface_name) const override;
};

} // namespace sampling
} // namespace bmon

#endif // NETSTAT_CMD_SAMPLER_H
