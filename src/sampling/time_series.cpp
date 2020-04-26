#include "time_series.hpp"
#include "macros.hpp"

namespace bandwit {
namespace sampling {

void TimeSeries::inc(TimePoint tp, uint64_t value) {
    std::size_t key = calculate_key(tp);
    auto current_value = ((key <= max_key_) && (size() > 0)) ? get_key(key) : 0;
    set_key(key, current_value + value);
}

void TimeSeries::set(TimePoint tp, uint64_t value) {
    std::size_t key = calculate_key(tp);
    set_key(key, value);
}

uint64_t TimeSeries::get(TimePoint tp) const {
    std::size_t key = calculate_key(tp);
    return get_key(key);
}

void TimeSeries::set_key(std::size_t key, uint64_t value) {
    // We can't use operator[] to set an element that doesn't exist in the
    // vector yet so we need to enlarge it proactively. The new slots will be
    // filled with zeroes, which is our null value anyway.
    if (key >= storage_.capacity()) {
        // double the vector size
        storage_.resize((key + 1) * 2);
    }

    storage_[key] = value;

    max_key_ = key;
    size_ = key + 1;
}

uint64_t TimeSeries::get_key(std::size_t key) const { return storage_.at(key); }

TimeSeriesSlice TimeSeries::get_slice_from_end(std::size_t len) const {
    auto last_key = max_key_;
    auto first_key = len > size() ? 0 : last_key + 1 - len;

    std::vector<TimePoint> time_points(last_key + 1 - first_key);
    std::vector<uint64_t> values(time_points.size());
    std::size_t i = 0;

    for (auto cursor = first_key; cursor <= last_key; ++cursor) {
        auto tp = reverse_key(cursor);
        auto value = get_key(cursor);

        time_points[i] = tp;
        // does not take into account the agg_interval
        values[i] = value;

        ++i;
    }

    auto agg_interval = aggregation_interval();
    TimeSeriesSlice slice{time_points, values, agg_interval};
    return slice;
}

AggregationInterval TimeSeries::aggregation_interval() const {
    Millis one_sec{1000};
    uint32_t multiplier = sampling_interval_ / one_sec;
    // this could easily fail
    return static_cast<AggregationInterval>(multiplier);
}

std::size_t TimeSeries::size() const { return size_; }

std::size_t TimeSeries::capacity() const { return storage_.capacity(); }

std::size_t TimeSeries::calculate_key(TimePoint tp) const {
    auto distance = (tp - start_);
    auto num_units = distance / sampling_interval_;
    return num_units;
}

TimePoint TimeSeries::reverse_key(std::size_t index) const {
    auto tp = start_ + (sampling_interval_ * index);
    return tp;
}

} // namespace sampling
} // namespace bandwit