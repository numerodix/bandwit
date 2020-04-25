#ifndef ALIASES_H
#define ALIASES_H

#include <chrono>

namespace bandwit {

using Clock = std::chrono::system_clock;
using TimePoint = Clock::time_point;
using Millis = std::chrono::milliseconds;

} // namespace bandwit

#endif // ALIASES_H