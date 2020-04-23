#ifndef BAR_CHART_H
#define BAR_CHART_H

#include <cstdint>
#include <vector>

#include "formatter.hpp"
#include "time_series_slice.hpp"
#include "termui/dimensions.hpp"

namespace bmon {
namespace termui {

class TerminalSurface;

class BarChart {
  public:
    explicit BarChart(TerminalSurface *surface) : surface_{surface} {}
    void draw_bars_from_right(const std::string& title, TimeSeriesSlice slice);
    void draw_yaxis(const Dimensions &dim, uint64_t max_value);
    void draw_xaxis(const Dimensions &dim, TimeSeriesSlice slice);
    void draw_title(const std::string& title);

    uint16_t get_width() const;

  private:
    TerminalSurface *surface_{nullptr};
    Formatter formatter_{};

    // 4 digits, a space, 4 chars, a space to delimit
    uint16_t scale_width_{10};
};

} // namespace termui
} // namespace bmon

#endif // BAR_CHART_H
