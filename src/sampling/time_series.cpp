#include "time_series.hpp"
#include "logging.hpp"
#include "macros.hpp"

namespace bandwit {
namespace sampling {

void TimeSeries::inc(TimePoint tp, uint64_t value) {
    // are we well over size?
    if (F64(size()) > F64(max_capacity_) * oversize_factor_) {
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

TimeSeriesSlice TimeSeries::get_slice_from_point(TimePoint tp, std::size_t len,
                                                 Statistic stat) const {
    auto last_key = calculate_key(tp);
    auto first_key = len > (last_key + 1) ? 0 : last_key + 1 - len;

    LOG_A("first_key: %ld\n", first_key);
    LOG_A("last_key: %ld\n", last_key);
    LOG_A("max_key: %ld\n", max_key_);

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

TimePoint TimeSeries::min() const { return start_; }

TimePoint TimeSeries::max() const { return reverse_key(max_key_); }

std::optional<TimePoint> TimeSeries::minus_one(TimePoint tp) const {
    int key = INT(calculate_key(tp));
    --key;

    if ((key < 0) || (key > INT(max_key_))) {
        return std::nullopt;
    }

    TimePoint res = reverse_key(SIZE_T(key));
    return std::optional<TimePoint>(res);
}

std::optional<TimePoint> TimeSeries::plus_one(TimePoint tp) const {
    int key = INT(calculate_key(tp));
    ++key;

    if ((key < 0) || (key > INT(max_key_))) {
        return std::nullopt;
    }

    TimePoint res = reverse_key(SIZE_T(key));
    return std::optional<TimePoint>(res);
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