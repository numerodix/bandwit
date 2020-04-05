#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "bar_chart.h"

namespace bmon {
namespace termui {

void BarChart::draw_bars_from_right(std::vector<uint64_t> values) {
    Dimensions dim = display_->get_dimensions();

    std::vector<uint64_t>::iterator res;
    res = std::max_element(values.begin(), values.end());
    uint64_t max = *res;

    std::vector<uint16_t> scaled{};
    for (auto it = values.rbegin(); it != values.rend(); ++it) {
        double perc = static_cast<double>(*it) / static_cast<double>(max);
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

    display_->redraw();
}

} // namespace termui
} // namespace bmon
