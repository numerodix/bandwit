#ifndef SAMPLER_DETECTOR_H
#define SAMPLER_DETECTOR_H

#include <memory>
#include <string>

#include "sampling/detection_result.hpp"
#include "sampling/sample.hpp"
#include "sampling/sampler.hpp"

namespace bandwit {
namespace sampling {

class SamplerDetector {
  public:
    DetectionResult detect_sampler(const std::string &iface_name) const;
};

} // namespace sampling
} // namespace bandwit

#endif // SAMPLER_DETECTOR_H
