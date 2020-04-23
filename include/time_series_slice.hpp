#ifndef TIME_SERIES_SLICE_H
#define TIME_SERIES_SLICE_H

#include <unistd.h>
#include <vector>

#include "aliases.hpp"

class TimeSeriesSlice {
  public:
    TimeSeriesSlice(std::vector<TimePoint> tps, std::vector<uint64_t> vals)
      : time_points{tps}, values{vals} {}

    std::vector<TimePoint> time_points;
    std::vector<uint64_t> values;
};

#endif // TIME_SERIES_SLICE_H