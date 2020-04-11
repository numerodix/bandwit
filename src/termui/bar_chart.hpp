#ifndef BAR_CHART_H
#define BAR_CHART_H

#include <cstdint>
#include <vector>

#include "termui/dimensions.hpp"

namespace bmon {
namespace termui {

class TerminalSurface;

class BarChart {
  public:
    BarChart(TerminalSurface *surface) : surface_{surface} {}
    void draw_bars_from_right(std::vector<uint64_t> values);
    void draw_scale(const Dimensions &dim, uint64_t max_value);
    void draw_legend(uint64_t avg, uint64_t max, uint64_t last);

    uint16_t get_width() const;

  private:
    TerminalSurface *surface_{nullptr};

    // 4 digits, a space, 4 chars, a space to delimit
    uint16_t scale_width_{10};
};

} // namespace termui
} // namespace bmon

#endif // BAR_CHART_H