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

TimeSeriesSlice
TimeSeriesCollection::get_slice_from_point(AggregationWindow window,
                                           TimePoint tp, std::size_t len,
                                           Statistic stat) const {
    const auto &ts = coll_.at(window);
    return ts->get_slice_from_point(tp, len, stat);
}

TimePoint TimeSeriesCollection::min(AggregationWindow window) const {
    const auto &ts = coll_.at(window);
    return ts->min();
}

TimePoint TimeSeriesCollection::max(AggregationWindow window) const {
    const auto &ts = coll_.at(window);
    return ts->max();
}

std::optional<TimePoint>
TimeSeriesCollection::minus_one(AggregationWindow window, TimePoint tp) const {
    const auto &ts = coll_.at(window);
    return ts->minus_one(tp);
}

std::optional<TimePoint>
TimeSeriesCollection::plus_one(AggregationWindow window, TimePoint tp) const {
    const auto &ts = coll_.at(window);
    return ts->plus_one(tp);
}

std::size_t TimeSeriesCollection::size(AggregationWindow window) const {
    const auto &ts = coll_.at(window);
    return ts->size();
}

} // namespace sampling
} // namespace bandwit