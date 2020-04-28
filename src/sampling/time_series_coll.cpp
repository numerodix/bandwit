#include <chrono>

#include "macros.hpp"
#include "time_series_coll.hpp"

namespace bandwit {
namespace sampling {

TimeSeriesCollection::TimeSeriesCollection(
    TimePoint tp, const std::vector<AggregationWindow> &windows) {
    std::chrono::seconds one_sec{1};

    for (const auto window : windows) {
        auto interval = one_sec * INT(window);
        coll_[window] = std::make_unique<TimeSeries>(interval, tp);
    }
}

void TimeSeriesCollection::inc(TimePoint tp, uint64_t value) {
    for (const auto &pair : coll_) {
        const auto &ts = pair.second;
        ts->inc(tp, value);
    }
}

TimeSeriesSlice TimeSeriesCollection::get_slice_from_end(
    AggregationWindow window, std::size_t len, Statistic stat) const {
    const auto &ts = coll_.at(window);
    return ts->get_slice_from_end(len, stat);
}

} // namespace sampling
} // namespace bandwit