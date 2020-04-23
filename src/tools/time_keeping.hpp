#ifndef TIME_KEEPING_H
#define TIME_KEEPING_H

#include "aliases.hpp"

namespace bmon {
namespace tools {

class TimeKeeping {
  public:
    int get_hours(TimePoint tp);
    int get_minutes(TimePoint tp);
    int get_seconds(TimePoint tp);
};

} // namespace tools
} // namespace bmon

#endif // TIME_KEEPING_H

