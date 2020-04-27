#include <iomanip>
#include <iostream>
#include <sstream>

#include "formatter.hpp"
#include "macros.hpp"

namespace bandwit {
namespace termui {

const std::string& FormattedString::get() const {
    return str_;
}

std::size_t FormattedString::size() const {
    bool in_escape = false;
    std::size_t count{0};

    for (auto ch: str_) {
        if ((!in_escape) && (ch == '\033')) {
            in_escape = true;
            continue;
        } else if (in_escape && (ch == 'm')) {
            in_escape = false;
            continue;
        }

        if (!in_escape) {
            ++count;
        }
    }

    return count;
}

std::string Formatter::format_num_byte_rate(uint64_t num,
                                            const std::string &time_unit) {
    uint64_t int_part = 0;
    uint64_t dec_part = 0;
    std::string unit = "b";

    for (auto it = units_.rbegin(); it != units_.rend(); ++it) {
        auto pair = *it;
        auto exponent = U64(pair.first);

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
        double reconstructed = F64(int_part) + F64(dec_part) / 1000.0;

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
    ss << std::setw(4); // right align numbers
    ss << truncated << " " << unit << "/" << time_unit;
    return ss.str();
}

FormattedString Formatter::format_xaxis_per_sec(std::vector<TimePoint> points) {
    std::stringstream ss{};

    // If we need to write more than one char for a given point then successive
    // iterations through the loop will need to skip outputing anything at all
    // to make up for the space used.
    int chars_to_skip{0};

    int num_chars_after_this_one{-1};

    for (std::size_t i = 0; i < points.size(); i++) {
        num_chars_after_this_one = points.size() - 1 - i;

        auto tp = points[i];
        int secs = time_keeping_.get_seconds(tp);

        if (chars_to_skip > 0) {
            chars_to_skip--;
            continue;
        }

        if ((secs == 0) && (num_chars_after_this_one >= 4)) {
            // We need to output HH:MM
            auto tick = format_HH_MM(tp);
            ss << bold(tick);
            chars_to_skip = 4;
        } else if ((secs % 4 == 0) && (num_chars_after_this_one >= 1)) {
            // We need to output SS
            auto tick = format_SS(tp);
            ss << tick;
            chars_to_skip = 1;
        } else {
            ss << ' ';
        }
    }

    return FormattedString{ss.str()};
}

FormattedString Formatter::format_xaxis_per_min(std::vector<TimePoint> points) {
    std::stringstream ss{};

    // If we need to write more than one char for a given point then successive
    // iterations through the loop will need to skip outputing anything at all
    // to make up for the space used.
    int chars_to_skip{0};

    int num_chars_after_this_one{-1};

    for (std::size_t i = 0; i < points.size(); i++) {
        num_chars_after_this_one = points.size() - 1 - i;

        auto tp = points[i];
        int mins = time_keeping_.get_minutes(tp);

        if (chars_to_skip > 0) {
            chars_to_skip--;
            continue;
        }

        if ((mins == 0) && (num_chars_after_this_one >= 2)) {
            // We need to output HHh
            auto tick = format_HH_h(tp);
            ss << bold(tick);
            chars_to_skip = 2;
        } else if ((mins % 4 == 0) && (num_chars_after_this_one >= 1)) {
            // We need to output MM
            auto tick = format_MM(tp);
            ss << tick;
            chars_to_skip = 1;
        } else {
            ss << ' ';
        }
    }

    return FormattedString{ss.str()};
}

FormattedString Formatter::format_xaxis_per_hour(std::vector<TimePoint> points) {
    std::stringstream ss{};

    // If we need to write more than one char for a given point then successive
    // iterations through the loop will need to skip outputing anything at all
    // to make up for the space used.
    int chars_to_skip{0};

    int num_chars_after_this_one{-1};

    for (std::size_t i = 0; i < points.size(); i++) {
        num_chars_after_this_one = points.size() - 1 - i;

        auto tp = points[i];
        int hours = time_keeping_.get_hours(tp);

        if (chars_to_skip > 0) {
            chars_to_skip--;
            continue;
        }

        if ((hours == 0) && (num_chars_after_this_one >= 2)) {
            // We need to output Fri
            auto tick = format_Day(tp);
            ss << bold(tick);
            chars_to_skip = 2;
        } else if ((hours % 4 == 0) && (num_chars_after_this_one >= 1)) {
            // We need to output HH
            auto tick = format_HH(tp);
            ss << tick;
            chars_to_skip = 1;
        } else {
            ss << ' ';
        }
    }

    return FormattedString{ss.str()};
}

FormattedString Formatter::format_xaxis_per_day(std::vector<TimePoint> points) {
    std::stringstream ss{};

    // If we need to write more than one char for a given point then successive
    // iterations through the loop will need to skip outputing anything at all
    // to make up for the space used.
    int chars_to_skip{0};

    int num_chars_after_this_one{-1};

    for (std::size_t i = 0; i < points.size(); i++) {
        num_chars_after_this_one = points.size() - 1 - i;

        auto tp = points[i];
        int day = time_keeping_.get_wday(tp);

        if (chars_to_skip > 0) {
            chars_to_skip--;
            continue;
        }

        if ((day == 1) && (num_chars_after_this_one >= 2)) {
            // We need to output Mon
            auto tick = format_Day(tp);
            ss << tick;
            chars_to_skip = 2;
        } else {
            ss << ' ';
        }
    }

    return FormattedString{ss.str()};
}

std::string Formatter::format_Day(TimePoint tp) {
    int wday = time_keeping_.get_wday(tp);

    switch (wday) {
    case 0:
        return "Sun";
    case 1:
        return "Mon";
    case 2:
        return "Tue";
    case 3:
        return "Wed";
    case 4:
        return "Thu";
    case 5:
        return "Fri";
    case 6:
        return "Sat";
    }
}

std::string Formatter::format_HH_MM(TimePoint tp) {
    int hours = time_keeping_.get_hours(tp);
    int mins = time_keeping_.get_minutes(tp);

    std::stringstream ss{};
    ss << std::setfill('0') << std::setw(2) << hours;
    ss << ':';
    ss << std::setfill('0') << std::setw(2) << mins;
    return ss.str();
}

std::string Formatter::format_HH_h(TimePoint tp) {
    int hours = time_keeping_.get_hours(tp);

    std::stringstream ss{};
    ss << std::setfill('0') << std::setw(2) << hours << 'h';
    return ss.str();
}

std::string Formatter::format_HH(TimePoint tp) {
    int hours = time_keeping_.get_hours(tp);

    std::stringstream ss{};
    ss << std::setfill('0') << std::setw(2) << hours;
    return ss.str();
}

std::string Formatter::format_MM(TimePoint tp) {
    int mins = time_keeping_.get_minutes(tp);

    std::stringstream ss{};
    ss << std::setfill('0') << std::setw(2) << mins;
    return ss.str();
}

std::string Formatter::format_SS(TimePoint tp) {
    int secs = time_keeping_.get_seconds(tp);

    std::stringstream ss{};
    ss << std::setfill('0') << std::setw(2) << secs;
    return ss.str();
}

std::string Formatter::bold(const std::string& str) {
    std::stringstream ss{};
    ss << ansi_bold << str << ansi_reset_;
    return ss.str();
}

std::string Formatter::reverse_video(const std::string& str) {
    std::stringstream ss{};
    ss << ansi_reverse_video_ << str << ansi_reset_;
    return ss.str();
}

} // namespace termui
} // namespace bandwit
