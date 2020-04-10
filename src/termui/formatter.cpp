#include <iomanip>
#include <iostream>
#include <sstream>

#include "formatter.h"

namespace bmon {
namespace termui {

std::string Formatter::format_num_byte_rate(uint64_t num,
                                            const std::string &time_unit) {
    std::stringstream ss{};
    ss << std::setw(3); // right align numbers

    if (num > (1UL << 30UL)) {
        ss << (num >> 30UL) << " gb/" << time_unit;
    } else if (num > (1UL << 20UL)) {
        ss << (num >> 20UL) << " mb/" << time_unit;
    } else if (num > (1UL << 10UL)) {
        ss << (num >> 10UL) << " kb/" << time_unit;
    } else {
        ss << num << " b/" << time_unit;
    }

    return ss.str();
}

} // namespace termui
} // namespace bmon
