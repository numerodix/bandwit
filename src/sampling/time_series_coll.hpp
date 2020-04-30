#ifndef TIME_SERIES_COLL_H
#define TIME_SERIES_COLL_H

#include <memory>
#include <unistd.h>
#include <unordered_map>
#include <vector>

#include "aliases.hpp"
#include "sampling/agg_window.hpp"
#include "sampling/statistic.hpp"
#include "sampling/time_series_slice.hpp"
#include "time_series.hpp"

namespace bandwit {
namespace sampling {

class TimeSeriesCollection {
  public:
    explicit TimeSeriesCollection(
        TimePoint tp, const std::vector<AggregationWindow> &windows);

    void inc(TimePoint tp, uint64_t value);
    TimeSeriesSlice get_slice_from_pos(AggregationWindow window,
                                       std::size_t pos, std::size_t len,
                                       Statistic stat) const;
    std::size_t size(AggregationWindow window) const;

  private:
    std::unordered_map<AggregationWindow, std::unique_ptr<TimeSeries>> coll_{};
};

} // namespace sampling
} // namespace bandwit

#endif // TIME_SERIES_COLL_H
