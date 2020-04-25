#ifndef DETECTION_RESULT_H
#define DETECTION_RESULT_H

#include "sample.hpp"
#include "sampler.hpp"

namespace bandwit {
namespace sampling {

struct DetectionResult {
    // could this be a unique_ptr?
    Sampler *sampler{nullptr};
    Sample sample{};
};

} // namespace sampling
} // namespace bandwit

#endif // DETECTION_RESULT_H
