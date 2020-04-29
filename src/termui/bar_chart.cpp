#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <sstream>

#include "bar_chart.hpp"
#include "macros.hpp"
#include "terminal_surface.hpp"

namespace bandwit {
namespace termui {

// ref: https://en.wikipedia.org/wiki/Box-drawing_character

void BarChart::draw_bars_from_right(const std::string &iface_name,
                                    const std::string &title,
                                    const TimeSeriesSlice &slice,
                                    DisplayScale scale, Statistic stat) {
    auto dim = surface_->get_size();
    std::vector<uint16_t> scaled{};

    auto max = std::max_element(slice.values.begin(), slice.values.end());
    uint64_t max_value = *max;

    if (scale == DisplayScale::LINEAR) {

        for (auto it = slice.values.rbegin(); it != slice.values.rend(); ++it) {
            double perc = F64(*it) / F64(max_value);
            auto magnitude = U64(perc * F64(dim.height - chart_offset_));
            scaled.push_back(magnitude);
        }

    } else if (scale == DisplayScale::LOG10) {

        for (auto it = slice.values.rbegin(); it != slice.values.rend(); ++it) {
            auto magnitude = U64(std::log10(F64(*it)) + 1);
            scaled.push_back(magnitude);
        }

    } else if (scale == DisplayScale::LOG2) {

        for (auto it = slice.values.rbegin(); it != slice.values.rend(); ++it) {
            auto magnitude = U64(std::log2(F64(*it)) + 1);
            scaled.push_back(magnitude);
        }
    }

    surface_->clear_surface();

    uint16_t col_cur = dim.width;
    uint16_t bottom_edge = dim.height - chart_offset_;
    uint16_t vertical_space = bottom_edge;
    for (auto value : scaled) {
        if (value == 0) {
            Point pt{col_cur, bottom_edge};
            surface_->put_uchar(pt, u8"▁");
        }

        for (uint16_t j = 0; j < value && j < vertical_space; ++j) {
            uint16_t y = bottom_edge - j;
            Point pt{col_cur, y};
            surface_->put_uchar(pt, u8"█");
        }

        --col_cur;
    }

    draw_yaxis(dim, max_value, scale, stat);
    draw_xaxis(dim, slice);
    draw_yaxis_label(dim, scale);
    draw_title(title, slice, stat);
    draw_menu(iface_name, dim);

    surface_->flush();
}

void BarChart::draw_yaxis(const Dimensions &dim, uint64_t max_value,
                          DisplayScale scale, Statistic stat) {
    std::vector<uint64_t> ticks{};
    std::vector<std::string> ticks_fmt{};
    YAxisScale y_scale = YAxisScale::BASE2;

    if (scale == DisplayScale::LINEAR) {

        double factor = 1.0 / F64(dim.height);
        for (int x = 1; x <= dim.height - chart_offset_; ++x) {
            auto tick = U64(F64(max_value) * (x * factor));
            ticks.push_back(tick);
        }

    } else if (scale == DisplayScale::LOG10) {
        y_scale = YAxisScale::BASE10;

        for (int x = 0; x < dim.height - chart_offset_; ++x) {
            double tick = std::pow(10.0, F64(x));
            if (tick < std::numeric_limits<uint64_t>::max()) {
                ticks.push_back(U64(tick));
            }
        }

    } else if (scale == DisplayScale::LOG2) {

        for (int x = 0; x < dim.height - chart_offset_; ++x) {
            double tick = std::pow(2.0, F64(x));
            if (tick < std::numeric_limits<uint64_t>::max()) {
                ticks.push_back(U64(tick));
            }
        }
    }

    for (const auto &tick : ticks) {
        std::string tick_fmt{};
        if (stat == Statistic::AVERAGE) {
            tick_fmt = formatter_.format_num_bytes_rate(y_scale, tick, "s");
        } else if (stat == Statistic::SUM) {
            tick_fmt = formatter_.format_num_bytes(y_scale, tick);
        }
        ticks_fmt.push_back(std::move(tick_fmt));
    }

    uint16_t row_cur = dim.height - chart_offset_;
    for (const auto &tick_fmt : ticks_fmt) {
        Point pt{1, row_cur--};
        surface_->put_string(pt, tick_fmt);
    }
}

void BarChart::draw_xaxis(const Dimensions &dim, const TimeSeriesSlice &slice) {
    FormattedString axis{};

    switch (slice.agg_window) {
    case AggregationWindow::ONE_SECOND:
        axis = formatter_.format_xaxis_per_sec(slice.time_points);
        break;
    case AggregationWindow::ONE_MINUTE:
        axis = formatter_.format_xaxis_per_min(slice.time_points);
        break;
    case AggregationWindow::ONE_HOUR:
        axis = formatter_.format_xaxis_per_hour(slice.time_points);
        break;
    case AggregationWindow::ONE_DAY:
        axis = formatter_.format_xaxis_per_day(slice.time_points);
        break;
    }

    uint16_t col = dim.width - axis.size() + 1;
    auto y = U16(dim.height - xaxis_offset_);

    Point pt{col, y};
    surface_->put_string(pt, axis.get());
}

void BarChart::draw_yaxis_label(const Dimensions &dim, DisplayScale scale) {
    std::string label = get_label(scale);

    std::stringstream ss{};
    ss << "<" << label << ">";
    std::string label_fmt = ss.str();

    auto col = U16((INT(scale_width_) / 2) - (INT(label_fmt.size()) / 2));
    uint16_t y = dim.height - 1;

    Point pt{col, y};
    surface_->put_string(pt, label_fmt);
}

void BarChart::draw_title(const std::string &title,
                          const TimeSeriesSlice &slice, Statistic stat) {
    auto dim = surface_->get_size();
    auto interval_label = sampling::get_label(slice.agg_window);
    auto stat_label = sampling::get_label(stat);

    std::stringstream ss{};
    ss << "[" << stat_label << " " << title << "/" << interval_label << "]";
    std::string title_fmt = ss.str();

    auto col = U16((INT(dim.width) / 2) - (INT(title_fmt.size()) / 2));
    uint16_t y = 1;

    Point pt{col, y};
    surface_->put_string(pt, title_fmt);
}

void BarChart::draw_menu(const std::string &iface_name, const Dimensions &dim) {
    std::string menu{" (q)uit (r)x (t)x s(c)ale (s)tat (up/down arrow)"};
    menu.resize(dim.width, ' ');

    // format iface
    std::stringstream ss{};
    ss << "[" << iface_name << "]";
    std::string iface_label = ss.str();

    // insert iface at the end
    auto to_index = menu.size() - 0;
    auto from_index = to_index - iface_label.size();
    menu.replace(from_index, to_index, iface_label);

    uint16_t col = 1;
    uint16_t y = dim.height;

    std::string menu_fmt = formatter_.reverse_video(menu);

    Point pt{col, y};
    surface_->put_string(pt, menu_fmt);
}

uint16_t BarChart::get_width() const {
    auto dim = surface_->get_size();
    return dim.width - scale_width_;
}

} // namespace termui
} // namespace bandwit