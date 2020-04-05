#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>

#include "bar_chart.h"
#include "formatter.h"

namespace bmon {
namespace termui {

void BarChart::draw_bars_from_right(std::vector<uint64_t> values) {
    Dimensions dim = display_->get_dimensions();

    auto max = std::max_element(values.begin(), values.end());
    uint64_t max_value = *max;
    uint64_t sum = std::accumulate(values.begin(), values.end(), U64(0));
    auto avg_value = U64(F64(sum) / F64(values.size()));
    uint64_t last_value = values.at(values.size() - 1);

    std::vector<uint16_t> scaled{};
    for (auto it = values.rbegin(); it != values.rend(); ++it) {
        double perc = F64(*it) / F64(max_value);
        auto magnitude = U64(perc * F64(dim.height));
        scaled.push_back(magnitude);
    }

    display_->clear_screen();

    uint16_t col_cur = dim.width;
    for (auto value : scaled) {

        for (uint16_t j = 0; j < value; ++j) {
            uint16_t y = dim.height - 1 - j;
            Point pt{col_cur, y};
            display_->put_char(pt, '*');
        }

        --col_cur;
    }

    draw_legend(avg_value, max_value, last_value);

    display_->redraw();
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
            display_->put_char(pt, legend[i]);
        }

        row++;
    }
}

} // namespace termui
} // namespace bmon
