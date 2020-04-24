#ifndef TIME_SERIES_SLICE_H
#define TIME_SERIES_SLICE_H

#include <unistd.h>
#include <vector>

#include "aliases.hpp"

namespace bmon {
namespace sampling {

class TimeSeriesSlice {
  public:
    explicit TimeSeriesSlice(std::vector<TimePoint> tps, std::vector<uint64_t> vals)
     : time_points{std::move(tps)}, values{std::move(vals)} {}

    std::vector<TimePoint> time_points{};
    std::vector<uint64_t> values{};
};

} // namespace sampling
} // namespace bmon

#endif // TIME_SERIES_SLICE_H