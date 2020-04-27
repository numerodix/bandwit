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

class FormattedString {
  public:
    explicit FormattedString(std::string str) : str_{std::move(str)} {}
    FormattedString() {}

    const std::string &get() const;
    std::size_t size() const;

  private:
    std::string str_{};
};

class Formatter {
  public:
    std::string format_num_bytes(uint64_t num);
    std::string format_num_bytes_rate(uint64_t num,
                                      const std::string &time_unit);

    FormattedString format_xaxis_per_sec(std::vector<TimePoint> points);
    FormattedString format_xaxis_per_min(std::vector<TimePoint> points);
    FormattedString format_xaxis_per_hour(std::vector<TimePoint> points);
    FormattedString format_xaxis_per_day(std::vector<TimePoint> points);

    std::string format_Day(TimePoint tp);
    std::string format_HH_MM(TimePoint tp);
    std::string format_HH_h(TimePoint tp);
    std::string format_HH(TimePoint tp);
    std::string format_MM(TimePoint tp);
    std::string format_SS(TimePoint tp);

    std::string bold(const std::string &str);
    std::string reverse_video(const std::string &str);

  private:
    bandwit::tools::TimeKeeping time_keeping_{};

    std::string ansi_bold{"\033[1m"};
    std::string ansi_reverse_video_{"\033[7m"};
    std::string ansi_reset_{"\033[0m"};

    // powers of two
    std::map<int, std::string> units_ = {
        {0, "b"},   {10, "kb"}, {20, "mb"}, {30, "gb"}, {40, "tb"},
        {50, "pb"}, {60, "eb"},
    };
};

} // namespace termui
} // namespace bandwit

#endif // FORMATTER_H
