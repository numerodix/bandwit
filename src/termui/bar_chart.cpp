#include <algorithm>
#include <numeric>
#include <sstream>

#include "bar_chart.hpp"
#include "formatter.hpp"
#include "macros.hpp"
#include "terminal_surface.hpp"

namespace bmon {
namespace termui {

void BarChart::draw_bars_from_right(std::vector<uint64_t> values) {
    auto dim = surface_->get_size();

    auto max = std::max_element(values.begin(), values.end());
    uint64_t max_value = *max;

    std::vector<uint16_t> scaled{};
    for (auto it = values.rbegin(); it != values.rend(); ++it) {
        double perc = F64(*it) / F64(max_value);
        auto magnitude = U64(perc * F64(dim.height));
        scaled.push_back(magnitude);
    }

    surface_->clear_surface();

    uint16_t col_cur = dim.width;
    for (auto value : scaled) {

        for (uint16_t j = 0; j < value; ++j) {
            uint16_t y = dim.height - 1 - j;
            Point pt{col_cur, y};
            surface_->put_char(pt, '|');
        }

        --col_cur;
    }

    draw_scale(dim, max_value);

    surface_->flush();
}

void BarChart::draw_legend(uint64_t avg, uint64_t max, uint64_t last) {
    Formatter fmt{};

    std::vector<std::pair<std::string, uint64_t>> pairs{
        {"max ", max},
        {"avg ", avg},
        {"last", last},
    };

    uint16_t row = 0;

    for (auto pair : pairs) {
        std::stringstream ss{};
        std::string rate = fmt.format_num_byte_rate(pair.second, "s");
        ss << "[" << pair.first << ": " << rate << "]";
        std::string legend = ss.str();

        for (size_t i = 0; i < legend.size(); ++i) {
            uint16_t x = U16(i) + U16(1);
            Point pt{x, row};
            surface_->put_char(pt, legend[i]);
        }

        row++;
    }
}

void BarChart::draw_scale(const Dimensions &dim, uint64_t max_value) {
    Formatter fmt{};
    std::vector<std::string> ticks{};

    double factor = 1.0 / F64(dim.height);
    for (int x = 1; x <= dim.height; ++x) {
        auto tick = U64(F64(max_value) * (x * factor));
        std::string tick_fmt = fmt.format_num_byte_rate(tick, "s");
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

uint16_t BarChart::get_width() const {
    auto dim = surface_->get_size();
    return dim.width - scale_width_;
}

} // namespace termui
} // namespace bmon