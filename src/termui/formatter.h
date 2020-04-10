#ifndef FORMATTER_H
#define FORMATTER_H

#include <cstdint>
#include <map>
#include <string>

namespace bmon {
namespace termui {

class Formatter {
  public:
    std::string format_num_byte_rate(uint64_t num,
                                     const std::string &time_unit);

  private:
    // powers of two
    std::map<int, std::string> units_ = {
        {0, "b"},
        {10, "kb"},
        {20, "mb"},
        {30, "gb"},
    };
};

} // namespace termui
} // namespace bmon

#endif // FORMATTER_H
