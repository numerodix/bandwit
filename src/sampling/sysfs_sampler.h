#ifndef SYSFS_SAMPLER_H
#define SYSFS_SAMPLER_H

#include "sampling/sampler.h"

namespace bmon {
namespace sampling {

class SysFsSampler : public Sampler {

  public:
    SysFsSampler() = default;
    ~SysFsSampler() override = default;

    // disable copy/move constructors
    SysFsSampler(const SysFsSampler &) = delete;
    SysFsSampler(SysFsSampler &&) = delete;

    // disable assignment
    SysFsSampler &operator=(const SysFsSampler &) = delete;
    SysFsSampler &operator=(SysFsSampler &&) = delete;

    Sample get_sample(const std::string &iface_name) const override;
};

} // namespace sampling
} // namespace bmon

#endif // SYSFS_SAMPLER_H