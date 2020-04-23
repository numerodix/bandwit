#ifndef TIME_SERIES_H
#define TIME_SERIES_H

#include <chrono>
#include <vector>

namespace bmon {
namespace sampling {

class TimeSeries {
    using Ms = std::chrono::milliseconds;
    using TimePoint = std::chrono::system_clock::time_point;

  public:
    TimeSeries(Ms interval, TimePoint start)
        : interval_{interval}, start_{start} {}

    // convenience API using time points
    void set(TimePoint tp, uint64_t value);
    uint64_t get(TimePoint tp) const;

    // underlying API using vector indices
    void set_key(std::size_t key, uint64_t value);
    uint64_t get_key(std::size_t key) const;

    std::vector<uint64_t> get_slice_from_end(std::size_t len) const;
    std::size_t size() const;
    std::size_t capacity() const;

    std::size_t calculate_key(TimePoint tp) const;

  private:
    Ms interval_{};
    TimePoint start_{};

    std::vector<uint64_t> storage_{};
    std::size_t max_key_{0};
};

} // namespace sampling
} // namespace bmon

#endif // TIME_SERIES_H
