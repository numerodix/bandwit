#include "time_series.hpp"

namespace bmon {
namespace sampling {

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
}

uint64_t TimeSeries::get_key(std::size_t key) const { return storage_.at(key); }

std::vector<uint64_t> TimeSeries::get_slice_from_end(std::size_t len) const {
    auto last_key = max_key_;
    auto first_key = len > size() ? 0 : last_key - len + 1;

    std::vector<uint64_t> res(last_key - first_key + 1);
    for (auto cursor = first_key; cursor <= last_key; ++cursor) {
        auto value = get_key(cursor);
        res.emplace_back(value);
    }

    return res;
}

std::vector<std::pair<TimePoint, uint64_t>> TimeSeries::get_slice_from_end_(std::size_t len) const {
    auto last_key = max_key_;
    auto first_key = len > size() ? 0 : last_key - len + 1;

    std::vector<std::pair<TimePoint, uint64_t>> res(last_key - first_key + 1);
    for (auto cursor = first_key; cursor <= last_key; ++cursor) {
        auto tp = calculate_key(cursor);
        auto value = get_key(cursor);

        auto pair = std::make_pair(tp, value);
        res.emplace_back(pair);
    }

    return res;
}

TimeSeriesSlice TimeSeries::get_slice_from_end__(std::size_t len) const {
    auto last_key = max_key_;
    auto first_key = len > size() ? 0 : last_key - len + 1;

    std::vector<uint64_t> values(last_key - first_key + 1);
    std::vector<TimePoint> time_points(values.size());

    for (auto cursor = first_key; cursor <= last_key; ++cursor) {
        auto tp = calculate_key(cursor);
        auto value = get_key(cursor);

        time_points.emplace_back(tp);
        values.emplace_back(value);
    }

    TimeSeriesSlice slice{time_points, values};
    return slice;
}

std::size_t TimeSeries::size() const { return max_key_ + 1; }

std::size_t TimeSeries::capacity() const { return storage_.capacity(); }

std::size_t TimeSeries::calculate_key(TimePoint tp) const {
    auto distance = (tp - start_);
    auto num_units = distance / interval_;
    return num_units;
}

TimePoint TimeSeries::calculate_key(std::size_t index) const {
    auto tp = start_ + (interval_ * index);
    return tp;
}

} // namespace sampling
} // namespace bmon