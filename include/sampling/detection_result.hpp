#ifndef DETECTION_RESULT_H
#define DETECTION_RESULT_H

#include <memory>

#include "sample.hpp"
#include "sampler.hpp"

namespace bandwit {
namespace sampling {

struct DetectionResult {
    std::unique_ptr<Sampler> sampler;
    Sample sample;
};

} // namespace sampling
} // namespace bandwit

#endif // DETECTION_RESULT_H
