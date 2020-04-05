#include <iostream>
#include <sstream>

#include "formatter.h"

namespace bmon {
namespace termui {

std::string Formatter::format_num_byte_rate(uint64_t num,
                                            std::string time_unit) {
    std::stringstream ss{};

    if (num > (1UL << 30)) {
        ss << (num >> 30) << " gb/" << time_unit;
    } else if (num > (1UL << 20)) {
        ss << (num >> 20) << " mb/" << time_unit;
    } else if (num > (1UL << 10)) {
        ss << (num >> 10) << " kb/" << time_unit;
    } else {
        ss << num << " b/" << time_unit;
    }

    return ss.str();
}

} // namespace termui
} // namespace bmon
