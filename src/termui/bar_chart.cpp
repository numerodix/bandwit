#include <algorithm>
#include <numeric>
#include <sstream>

#include "bar_chart.hpp"
#include "macros.hpp"
#include "terminal_surface.hpp"

namespace bmon {
namespace termui {

// ref: https://en.wikipedia.org/wiki/Box-drawing_character

void BarChart::draw_bars_from_right(const std::string &title,
                                    TimeSeriesSlice slice) {
    auto dim = surface_->get_size();

    auto max = std::max_element(slice.values.begin(), slice.values.end());
    uint64_t max_value = *max;

    std::vector<uint16_t> scaled{};
    for (auto it = slice.values.rbegin(); it != slice.values.rend(); ++it) {
        double perc = F64(*it) / F64(max_value);
        auto magnitude = U64(perc * F64(dim.height - 1));
        scaled.push_back(magnitude);
    }

    surface_->clear_surface();

    uint16_t col_cur = dim.width;
    for (auto value : scaled) {

        for (uint16_t j = 0; j < value; ++j) {
            uint16_t y = dim.height - 1 - j;
            Point pt{col_cur, y};
            surface_->put_uchar(pt, u8"▊");
        }

        --col_cur;
    }

    draw_yaxis(dim, max_value);
    draw_xaxis(dim, slice);
    draw_title(title);

    surface_->flush();
}

void BarChart::draw_yaxis(const Dimensions &dim, uint64_t max_value) {
    std::vector<std::string> ticks{};

    double factor = 1.0 / F64(dim.height);
    for (int x = 1; x <= dim.height - 1; ++x) {
        auto tick = U64(F64(max_value) * (x * factor));
        std::string tick_fmt = formatter_.format_num_byte_rate(tick, "s");
        ticks.push_back(tick_fmt);
    }

    uint16_t row = dim.height - 1;

    for (auto tick : ticks) {
        for (size_t i = 0; i < tick.size(); ++i) {
            uint16_t x = U16(i) + U16(1);
            Point pt{x, row};
            surface_->put_char(pt, tick[i]);
        }

        row--;
    }
}

void BarChart::draw_xaxis(const Dimensions &dim, TimeSeriesSlice slice) {
    std::string axis = formatter_.format_xaxis(slice.time_points);

    uint16_t col_cur = dim.width - axis.size() + 1;
    for (std::size_t i = 0; i < axis.size(); ++i) {
        Point pt{col_cur++, dim.height};
        surface_->put_char(pt, axis[i]);
    }
}

void BarChart::draw_title(const std::string &title) {
    auto dim = surface_->get_size();

    std::stringstream ss{};
    ss << "[" << title << "]";
    std::string title_fmt = ss.str();

    auto x = U16((INT(dim.width) / 2) - (INT(title_fmt.size()) / 2));
    uint16_t y = 1;

    for (size_t i = 0; i < title_fmt.size(); ++i) {
        Point pt{x, y};
        surface_->put_char(pt, title_fmt[i]);
        x++;
    }
}

uint16_t BarChart::get_width() const {
    auto dim = surface_->get_size();
    return dim.width - scale_width_;
}

} // namespace termui
} // namespace bmon