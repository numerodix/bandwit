#include <algorithm>
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
                                    Statistic stat) {
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
    draw_title(title, slice, stat);
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

    uint16_t row_cur = dim.height - chart_offset_;
    for (auto tick : ticks) {
        Point pt{1, row_cur--};
        surface_->put_string(pt, tick);
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
    std::string menu{" (q)uit (r)x (t)x (s)tat (up/down arrow)"};
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