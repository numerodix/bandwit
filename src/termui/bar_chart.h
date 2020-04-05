#ifndef BAR_CHART_H
#define BAR_CHART_H

#include <cstdint>
#include <vector>

#include "display.h"

namespace bmon {
namespace termui {

class BarChart {
  public:
    explicit BarChart(Display *display) : display_{display} {}
    void draw_bars_from_right(std::vector<uint64_t> values);

  private:
    void draw_legend(uint64_t avg, uint64_t min, uint64_t max, uint64_t last);

    Display *display_{nullptr};
};

} // namespace termui
} // namespace bmon

#endif // BAR_CHART_H
