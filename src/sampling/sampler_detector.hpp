#ifndef SAMPLER_DETECTOR_H
#define SAMPLER_DETECTOR_H

#include <memory>
#include <string>

#include "sampling/sampler.hpp"

namespace bmon {
namespace sampling {

class SamplerDetector {
  public:
    std::unique_ptr<Sampler> detect_sampler(const std::string &iface_name) const;
};

} // namespace sampling
} // namespace bmon

#endif // SAMPLER_DETECTOR_H
