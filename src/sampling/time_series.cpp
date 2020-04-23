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

TimeSeriesSlice TimeSeries::get_slice_from_end(std::size_t len) const {
    auto last_key = max_key_;
    auto first_key = len > size() ? 0 : last_key - len + 1;

    std::vector<uint64_t> values(last_key - first_key + 1);
    for (auto cursor = first_key; cursor <= last_key; ++cursor) {
        auto value = get_key(cursor);
        values.emplace_back(value);
    }

    TimeSeriesSlice slice{values};
    return slice;
}

std::size_t TimeSeries::size() const { return max_key_ + 1; }

std::size_t TimeSeries::capacity() const { return storage_.capacity(); }

std::size_t TimeSeries::calculate_key(TimePoint tp) const {
    auto distance = (tp - start_);
    auto num_units = distance / interval_;
    return num_units;
}

} // namespace sampling
} // namespace bmon