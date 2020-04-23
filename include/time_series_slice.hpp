#ifndef TIME_SERIES_SLICE_H
#define TIME_SERIES_SLICE_H

#include <unistd.h>
#include <vector>

namespace bmon {

class TimeSeriesSlice {
  public:
    TimeSeriesSlice(std::vector<uint64_t> vals) : values{vals} {}

    std::vector<uint64_t> values;
};

} // namespace bmon

#endif // TIME_SERIES_SLICE_H