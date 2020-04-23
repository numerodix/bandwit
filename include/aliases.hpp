#ifndef ALIASES_H
#define ALIASES_H

#include <chrono>

namespace bmon {

using Clock = std::chrono::system_clock;
using TimePoint = Clock::time_point;
using Ms = std::chrono::milliseconds;

} // namespace bmon

#endif // ALIASES_H