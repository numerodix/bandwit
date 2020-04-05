#ifndef FORMATTER_H
#define FORMATTER_H

#include <cstdint>
#include <string>

namespace bmon {
namespace termui {

class Formatter {
  public:
    std::string format_num_byte_rate(uint64_t num,
                                     const std::string &time_unit);
};

} // namespace termui
} // namespace bmon

#endif // FORMATTER_H
