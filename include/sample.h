#ifndef SAMPLE_H
#define SAMPLE_H

#include <cstdint>
#include <ctime>

struct Sample {
    uint64_t rx;
    uint64_t tx;

    std::time_t ts;
};

#endif // SAMPLE_H