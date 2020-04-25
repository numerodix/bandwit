#ifndef PROCFS_SAMPLER_H
#define PROCFS_SAMPLER_H

#include "sampling/sampler.hpp"

namespace bandwit {
namespace sampling {

class ProcFsSampler : public Sampler {

  public:
    ProcFsSampler() = default;
    ~ProcFsSampler() override = default;

    CLASS_DISABLE_COPIES(ProcFsSampler)
    CLASS_DISABLE_MOVES(ProcFsSampler)

    Sample get_sample(const std::string &iface_name) const override;
};

} // namespace sampling
} // namespace bandwit

#endif // PROCFS_SAMPLER_H