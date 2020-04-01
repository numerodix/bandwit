#pragma once

#include <cstdint>
#include <ctime>


struct Sample {
    uint64_t rx;
    uint64_t tx;

    std::time_t ts;
};