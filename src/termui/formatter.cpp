#include <iomanip>
#include <iostream>
#include <sstream>

#include "formatter.h"
#include "macros.h"

namespace bmon {
namespace termui {

std::string Formatter::format_num_byte_rate(uint64_t num,
                                            const std::string &time_unit) {
    uint64_t int_part = 0;
    uint64_t dec_part = 0;
    std::string unit{};

    for (auto it = units_.rbegin(); it != units_.rend(); ++it) {
        auto pair = *it;
        uint64_t exponent = U64(pair.first);

        uint64_t val = num >> exponent;
        if (val > 0) {
            int_part = val;
            unit = pair.second;

            if (exponent >= 10) {
                uint64_t next_exponent = exponent - 10UL;
                dec_part = (num >> next_exponent) - (val << 10UL);
            }

            break;
        }
    }

    std::stringstream sd{};
    std::string truncated{};

    if (dec_part == 0) {
        // decimal part is zero, no need for a decimal part
        sd << int_part;
        truncated = sd.str();

    } else {
        // we need to glue together the int and dec parts
        double reconstructed = DOUBLE(int_part) + DOUBLE(dec_part) / 1000.0;

        sd.precision(3);
        sd << std::fixed << reconstructed;
        std::string formatted = sd.str();

        if (reconstructed >= 1000) {
            // 1023.45 -> 1023
            truncated = formatted.substr(0, 4);
        } else if (reconstructed >= 100) {
            // 123.456 -> 123
            truncated = formatted.substr(0, 3);
        } else {
            // 12.345 -> 12.3
            truncated = formatted.substr(0, 4);
        }
    }

    std::stringstream ss{};
    ss << std::setw(4);  // right align numbers
    ss << truncated << " " << unit << "/" << time_unit;
    return ss.str();
}

} // namespace termui
} // namespace bmon
