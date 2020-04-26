#ifndef TIME_SERIES_SLICE_H
#define TIME_SERIES_SLICE_H

#include <unistd.h>
#include <vector>

#include "agg_interval.hpp"
#include "aliases.hpp"

namespace bandwit {
namespace sampling {

// FIXME: should this be a struct?
class TimeSeriesSlice {
  public:
    explicit TimeSeriesSlice(std::vector<TimePoint> tps,
                             std::vector<uint64_t> vals,
                             AggregationInterval agg_int)
        : time_points{std::move(tps)}, values{std::move(vals)}, agg_interval{
                                                                    agg_int} {}

    std::vector<TimePoint> time_points{};
    std::vector<uint64_t> values{};
    AggregationInterval agg_interval;
};

} // namespace sampling
} // namespace bandwit

#endif // TIME_SERIES_SLICE_H