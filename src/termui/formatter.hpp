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
    std::string format_xaxis(std::vector<TimePoint> points);

    std::string format_hh_mm(TimePoint tp);
    std::string format_ss(TimePoint tp);

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
