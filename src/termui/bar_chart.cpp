#include <algorithm>
#include <numeric>
#include <sstream>

#include "bar_chart.hpp"
#include "macros.hpp"
#include "sampling/agg_interval.hpp"
#include "terminal_surface.hpp"

namespace bandwit {
namespace termui {

// ref: https://en.wikipedia.org/wiki/Box-drawing_character

void BarChart::draw_bars_from_right(const std::string &iface_name,
                                    const std::string &title,
                                    const TimeSeriesSlice &slice) {
    auto dim = surface_->get_size();

    auto max = std::max_element(slice.values.begin(), slice.values.end());
    uint64_t max_value = *max;

    std::vector<uint16_t> scaled{};
    for (auto it = slice.values.rbegin(); it != slice.values.rend(); ++it) {
        double perc = F64(*it) / F64(max_value);
        auto magnitude = U64(perc * F64(dim.height - chart_offset_));
        scaled.push_back(magnitude);
    }

    surface_->clear_surface();

    uint16_t col_cur = dim.width;
    for (auto value : scaled) {

        for (uint16_t j = 0; j < value; ++j) {
            uint16_t y = dim.height - chart_offset_ - j;
            Point pt{col_cur, y};
            surface_->put_uchar(pt, u8"▊");
        }

        --col_cur;
    }

    draw_yaxis(dim, max_value);
    draw_xaxis(dim, slice);
    draw_title(title, slice);
    draw_menu(iface_name, dim);

    surface_->flush();
}

void BarChart::draw_yaxis(const Dimensions &dim, uint64_t max_value) {
    std::vector<std::string> ticks{};

    double factor = 1.0 / F64(dim.height);
    for (int x = 1; x <= dim.height - chart_offset_; ++x) {
        auto tick = U64(F64(max_value) * (x * factor));
        std::string tick_fmt = formatter_.format_num_byte_rate(tick, "s");
        ticks.push_back(std::move(tick_fmt));
    }

    uint16_t row = dim.height - chart_offset_;

    for (auto tick : ticks) {
        for (size_t i = 0; i < tick.size(); ++i) {
            uint16_t x = U16(i) + U16(1);
            Point pt{x, row};
            surface_->put_char(pt, tick[i]);
        }

        row--;
    }
}

void BarChart::draw_xaxis(const Dimensions &dim, const TimeSeriesSlice &slice) {
    std::string axis{};

    switch (slice.agg_interval) {
    case sampling::AggregationInterval::ONE_SECOND:
        axis = formatter_.format_xaxis_per_sec(slice.time_points);
        break;
    case sampling::AggregationInterval::ONE_MINUTE:
        axis = formatter_.format_xaxis_per_min(slice.time_points);
        break;
    case sampling::AggregationInterval::ONE_HOUR:
        axis = formatter_.format_xaxis_per_hour(slice.time_points);
        break;
    case sampling::AggregationInterval::ONE_DAY:
        axis = formatter_.format_xaxis_per_day(slice.time_points);
        break;
    }

    uint16_t col_cur = dim.width - axis.size() + 1;
    for (auto ch : axis) {
        auto y = U16(dim.height - xaxis_offset_);
        Point pt{col_cur++, y};
        surface_->put_char(pt, ch);
    }
}

void BarChart::draw_title(const std::string &title,
                          const TimeSeriesSlice &slice) {
    auto dim = surface_->get_size();
    auto label = sampling::get_label(slice.agg_interval);

    std::stringstream ss{};
    ss << "[" << title << " per " << label << "]";
    std::string title_fmt = ss.str();

    auto x = U16((INT(dim.width) / 2) - (INT(title_fmt.size()) / 2));
    uint16_t y = 1;

    for (auto ch : title_fmt) {
        Point pt{x++, y};
        surface_->put_char(pt, ch);
    }
}

void BarChart::draw_menu(const std::string &iface_name, const Dimensions &dim) {
    std::string menu{" (q)uit (r)x (t)x (c)ycle"};
    menu.resize(dim.width, ' ');

    // format iface
    std::stringstream ss{};
    ss << "[" << iface_name << "]";
    std::string iface_label = ss.str();

    // insert iface at the end
    auto to_index = menu.size() - 0;
    auto from_index = to_index - iface_label.size();
    menu.replace(from_index, to_index, iface_label);

    uint16_t col_cur = 1;
    uint16_t y = dim.height;

    Point pt_start{col_cur, y};
    surface_->put_uchar(pt_start, "\033[7m");

    for (auto ch : menu) {
        Point pt{col_cur++, y};
        surface_->put_char(pt, ch);
    }

    Point pt_end{U16(col_cur - 1), y};
    surface_->put_uchar(pt_end, "\033[0m");
}

uint16_t BarChart::get_width() const {
    auto dim = surface_->get_size();
    return dim.width - scale_width_;
}

} // namespace termui
} // namespace bandwit