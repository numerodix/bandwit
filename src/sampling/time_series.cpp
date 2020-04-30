#include "time_series.hpp"
#include "macros.hpp"

namespace bandwit {
namespace sampling {

void TimeSeries::inc(TimePoint tp, uint64_t value) {
    // are we well over size?
    if (F64(size()) > F64(max_capacity_) * 1.5) {
        truncate();
    }

    std::size_t key = calculate_key(tp);
    auto current_value = ((key <= max_key_) && (size() > 0)) ? get_key(key) : 0;
    set_key(key, current_value + value);
}

uint64_t TimeSeries::get(TimePoint tp) const {
    std::size_t key = calculate_key(tp);
    return get_key(key);
}

void TimeSeries::set_key(std::size_t key, uint64_t value) {
    // We can't use operator[] to set an element that doesn't exist in the
    // vector yet so we need to enlarge it proactively. The new slots will be
    // filled with zeroes, which is our null value anyway.
    if (key >= storage_.size()) {
        // double the vector size
        storage_.resize((key + 1) * 2);
    }

    storage_[key] = value;

    // update invariants
    max_key_ = key > max_key_ ? key : max_key_;
    size_ = max_key_ + 1;
}

uint64_t TimeSeries::get_key(std::size_t key) const { return storage_.at(key); }

TimeSeriesSlice TimeSeries::get_slice_from_pos(std::size_t pos, std::size_t len,
                                               Statistic stat) const {
    auto last_key = max_key_ - pos;
    auto first_key = len > (last_key + 1) ? 0 : last_key + 1 - len;

    auto agg_window = aggregation_window();

    uint64_t divisor = 1;
    if (stat == Statistic::AVERAGE) {
        divisor = U64(agg_window);
    }

    std::vector<TimePoint> time_points(last_key + 1 - first_key);
    std::vector<uint64_t> values(time_points.size());
    std::size_t i = 0;

    for (auto cursor = first_key; cursor <= last_key; ++cursor) {
        auto tp = reverse_key(cursor);
        auto value = get_key(cursor);

        time_points[i] = tp;
        values[i] = value / divisor;

        ++i;
    }

    TimeSeriesSlice slice{time_points, values, agg_window};
    return slice;
}

AggregationWindow TimeSeries::aggregation_window() const {
    Millis one_sec{1000};
    uint32_t multiplier = sampling_interval_ / one_sec;

    // this will fail if sampling_interval_ does not match any
    // AggregationWindow
    return static_cast<AggregationWindow>(multiplier);
}

std::size_t TimeSeries::size() const { return size_; }

std::size_t TimeSeries::capacity() const { return storage_.capacity(); }

void TimeSeries::truncate() {
    int num_to_remove = size() - max_capacity_;
    storage_.erase(storage_.begin(), storage_.begin() + num_to_remove);

    // update invariants
    start_ += sampling_interval_ * num_to_remove;
    max_key_ -= num_to_remove;
    size_ -= num_to_remove;
}

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