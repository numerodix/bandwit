#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>

#include "bar_chart.h"

namespace bmon {
namespace termui {

void BarChart::draw_bars_from_right(std::vector<uint64_t> values) {
    Dimensions dim = display_->get_dimensions();

    auto [min, max] = std::minmax_element(values.begin(), values.end());
    uint64_t min_value = *min;
    uint64_t max_value = *max;
    uint64_t sum =
        std::accumulate(values.begin(), values.end(), static_cast<uint64_t>(0));
    auto avg_value = static_cast<uint64_t>(static_cast<double>(sum) /
                                           static_cast<double>(values.size()));
    uint64_t last_value = values.at(values.size() - 1);

    std::vector<uint16_t> scaled{};
    for (auto it = values.rbegin(); it != values.rend(); ++it) {
        double perc = static_cast<double>(*it) / static_cast<double>(max_value);
        auto magnitude =
            static_cast<uint16_t>(perc * static_cast<double>(dim.height));
        scaled.push_back(magnitude);
    }

    display_->clear_screen();

    uint16_t col_cur = dim.width - 1;
    for (auto value : scaled) {

        for (uint16_t j = 0; j < value; ++j) {
            uint16_t y = dim.height - 1 - j;
            Point pt{col_cur, y};
            display_->put_char(pt, '*');
        }

        --col_cur;
    }

    draw_legend(avg_value, min_value, max_value, last_value);

    display_->redraw();
}

void BarChart::draw_legend(uint64_t avg, uint64_t min, uint64_t max,
                           uint64_t last) {
    std::vector<std::pair<std::string, uint64_t>> pairs{
        {"max ", max},
        {"min ", min},
        {"avg ", avg},
        {"last", last},
    };

    uint16_t row = 0;

    for (auto pair : pairs) {
        std::stringstream ss{};
        ss << "[" << pair.first << ": " << pair.second << " B/s]";
        std::string legend = ss.str();

        for (size_t i = 0; i < legend.size(); ++i) {
            uint16_t x = static_cast<uint16_t>(i) + static_cast<uint16_t>(1);
            Point pt{x, row};
            display_->put_char(pt, legend[i]);
        }

        row++;
    }
}

} // namespace termui
} // namespace bmon
