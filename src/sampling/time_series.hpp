#ifndef TIME_SERIES_H
#define TIME_SERIES_H

#include <optional>
#include <vector>

#include "aliases.hpp"
#include "sampling/agg_window.hpp"
#include "sampling/statistic.hpp"
#include "sampling/time_series_slice.hpp"

namespace bandwit {
namespace sampling {

class TimeSeries {
  public:
    TimeSeries(Millis sampling_interval, TimePoint start)
        : sampling_interval_{sampling_interval}, start_{start} {}

    // convenience API using time points
    void inc(TimePoint tp, uint64_t value);
    uint64_t get(TimePoint tp) const;
    TimeSeriesSlice get_slice_from_point(TimePoint tp, std::size_t len,
                                         Statistic stat) const;

    TimePoint min() const;
    TimePoint max() const;
    std::optional<TimePoint> minus_one(TimePoint tp) const;
    std::optional<TimePoint> plus_one(TimePoint tp) const;

    // underlying API using vector indices
    void set_key(std::size_t key, uint64_t value);
    uint64_t get_key(std::size_t key) const;

    AggregationWindow aggregation_window() const;
    std::size_t size() const;
    std::size_t capacity() const;
    void truncate();

    std::size_t calculate_key(TimePoint tp) const;
    TimePoint reverse_key(std::size_t index) const;

  private:
    Millis sampling_interval_{};
    TimePoint start_{};
    // std::size_t max_capacity_{512};
    std::size_t max_capacity_{20};

    std::vector<uint64_t> storage_{};
    std::size_t max_key_{0};
    std::size_t size_{0};
};

} // namespace sampling
} // namespace bandwit

#endif // TIME_SERIES_H
