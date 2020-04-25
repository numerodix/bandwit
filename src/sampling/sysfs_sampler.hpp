#ifndef SYSFS_SAMPLER_H
#define SYSFS_SAMPLER_H

#include "sampling/sampler.hpp"

namespace bandwit {
namespace sampling {

class SysFsSampler : public Sampler {

  public:
    SysFsSampler() = default;
    ~SysFsSampler() override = default;

    CLASS_DISABLE_COPIES(SysFsSampler)
    CLASS_DISABLE_MOVES(SysFsSampler)

    Sample get_sample(const std::string &iface_name) const override;
};

} // namespace sampling
} // namespace bandwit

#endif // SYSFS_SAMPLER_H