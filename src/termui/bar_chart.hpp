#ifndef BAR_CHART_H
#define BAR_CHART_H

#include <cstdint>
#include <vector>

#include "formatter.hpp"
#include "sampling/agg_window.hpp"
#include "sampling/statistic.hpp"
#include "sampling/time_series_slice.hpp"
#include "termui/dimensions.hpp"

namespace bandwit {
namespace termui {

class TerminalSurface;

class BarChart {
    using AggregationWindow = sampling::AggregationWindow;
    using Statistic = sampling::Statistic;
    using TimeSeriesSlice = bandwit::sampling::TimeSeriesSlice;

  public:
    explicit BarChart(TerminalSurface *surface) : surface_{surface} {}
    void draw_bars_from_right(const std::string &iface_name,
                              const std::string &title,
                              const TimeSeriesSlice &slice, Statistic stat);
    void draw_yaxis(const Dimensions &dim, uint64_t max_value);
    void draw_xaxis(const Dimensions &dim, const TimeSeriesSlice &slice);
    void draw_title(const std::string &title, const TimeSeriesSlice &slice,
                    Statistic stat);
    void draw_menu(const std::string &iface_name, const Dimensions &dim);

    uint16_t get_width() const;

  private:
    TerminalSurface *surface_{nullptr};
    Formatter formatter_{};

    // 4 digits, a space, 4 chars, a space to delimit
    uint16_t scale_width_{10};

    // distances from dim.height
    uint16_t xaxis_offset_{1};
    uint16_t chart_offset_{2};
};

} // namespace termui
} // namespace bandwit

#endif // BAR_CHART_H
