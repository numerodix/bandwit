#include "time_keeping.hpp"

namespace bandwit {
namespace tools {

int TimeKeeping::get_hours(TimePoint tp) {
    std::time_t tt = Clock::to_time_t(tp);
    tm local_tm = *localtime(&tt);
    return local_tm.tm_hour;
}

int TimeKeeping::get_minutes(TimePoint tp) {
    std::time_t tt = Clock::to_time_t(tp);
    tm local_tm = *localtime(&tt);
    return local_tm.tm_min;
}

int TimeKeeping::get_seconds(TimePoint tp) {
    std::time_t tt = Clock::to_time_t(tp);
    tm local_tm = *localtime(&tt);
    return local_tm.tm_sec;
}

} // namespace tools
} // namespace bandwit
