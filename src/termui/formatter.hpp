#ifndef FORMATTER_H
#define FORMATTER_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "aliases.hpp"
#include "tools/time_keeping.hpp"

namespace bandwit {
namespace termui {

class Formatter {
  public:
    std::string format_num_byte_rate(uint64_t num,
                                     const std::string &time_unit);

    std::string format_xaxis_per_sec(std::vector<TimePoint> points);
    std::string format_xaxis_per_min(std::vector<TimePoint> points);
    std::string format_xaxis_per_hour(std::vector<TimePoint> points);
    std::string format_xaxis_per_day(std::vector<TimePoint> points);

    std::string format_Day(TimePoint tp);
    std::string format_HH_MM(TimePoint tp);
    std::string format_HH_h(TimePoint tp);
    std::string format_HH(TimePoint tp);
    std::string format_MM(TimePoint tp);
    std::string format_SS(TimePoint tp);

  private:
    bandwit::tools::TimeKeeping time_keeping_{};

    // powers of two
    std::map<int, std::string> units_ = {
        {0, "b"},
        {10, "kb"},
        {20, "mb"},
        {30, "gb"},
    };
};

} // namespace termui
} // namespace bandwit

#endif // FORMATTER_H
