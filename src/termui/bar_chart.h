#ifndef BAR_CHART_H
#define BAR_CHART_H

#include <cstdint>
#include <vector>

namespace bmon {
namespace termui {

class TerminalSurface;

class BarChart {
  public:
    BarChart(TerminalSurface *surface) : surface_{surface} {}
    void draw_bars_from_right(std::vector<uint64_t> values);
    void draw_legend(uint64_t avg, uint64_t max, uint64_t last);

  private:
    TerminalSurface *surface_{nullptr};
};

} // namespace termui
} // namespace bmon

#endif // BAR_CHART_H
