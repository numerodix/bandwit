#ifndef TIME_SERIES_SLICE_H
#define TIME_SERIES_SLICE_H

#include <unistd.h>
#include <vector>

#include "agg_window.hpp"
#include "aliases.hpp"

namespace bandwit {
namespace sampling {

// FIXME: should this be a struct?
class TimeSeriesSlice {
  public:
    explicit TimeSeriesSlice(std::vector<TimePoint> tps,
                             std::vector<uint64_t> vals,
                             AggregationWindow agg_win)
        : time_points{std::move(tps)}, values{std::move(vals)}, agg_window{
                                                                    agg_win} {}

    // We need this to be able to declare a variable in an outer scope and
    // populate it in an inner scope. The value should not be used for anything
    // cause it's going to be empty.
    TimeSeriesSlice() {}

    std::vector<TimePoint> time_points{};
    std::vector<uint64_t> values{};
    AggregationWindow agg_window;
};

} // namespace sampling
} // namespace bandwit

#endif // TIME_SERIES_SLICE_H