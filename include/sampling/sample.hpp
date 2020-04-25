#ifndef SAMPLE_H
#define SAMPLE_H

#include <cstdint>
#include <ctime>

namespace bandwit {
namespace sampling {

struct Sample {
    uint64_t rx;
    uint64_t tx;

    std::time_t ts;
};

} // namespace sampling
} // namespace bandwit

#endif // SAMPLE_H
