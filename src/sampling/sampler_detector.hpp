#ifndef SAMPLER_DETECTOR_H
#define SAMPLER_DETECTOR_H

#include <memory>
#include <string>

#include "sampling/sample.hpp"
#include "sampling/sampler.hpp"

namespace bandwit {
namespace sampling {

struct DetectionResult {
    // could this be a unique_ptr?
    Sampler *sampler{nullptr};
    Sample sample{};
};

class SamplerDetector {
  public:
    DetectionResult detect_sampler(const std::string &iface_name) const;
};

} // namespace sampling
} // namespace bandwit

#endif // SAMPLER_DETECTOR_H
