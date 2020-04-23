#ifndef ALIASES_H
#define ALIASES_H

#include <chrono>

using Clock = std::chrono::system_clock;
using TimePoint = Clock::time_point;
using Ms = std::chrono::milliseconds;

#endif // ALIASES_H