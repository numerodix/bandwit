#include "sampling/agg_interval.hpp"

namespace bandwit {
namespace sampling {

AggregationInterval next_interval(AggregationInterval agg_interval) {
    switch (agg_interval) {
    case AggregationInterval::ONE_SECOND:
        return AggregationInterval::ONE_MINUTE;
    case AggregationInterval::ONE_MINUTE:
        return AggregationInterval::ONE_HOUR;
    case AggregationInterval::ONE_HOUR:
        return AggregationInterval::ONE_DAY;
    case AggregationInterval::ONE_DAY:
        return AggregationInterval::ONE_DAY;
    }
}

AggregationInterval prev_interval(AggregationInterval agg_interval) {
    switch (agg_interval) {
    case AggregationInterval::ONE_DAY:
        return AggregationInterval::ONE_HOUR;
    case AggregationInterval::ONE_HOUR:
        return AggregationInterval::ONE_MINUTE;
    case AggregationInterval::ONE_MINUTE:
        return AggregationInterval::ONE_SECOND;
    case AggregationInterval::ONE_SECOND:
        return AggregationInterval::ONE_SECOND;
    }
}

std::string get_label(AggregationInterval agg_interval) {
    switch (agg_interval) {
    case AggregationInterval::ONE_SECOND:
        return "sec";
    case AggregationInterval::ONE_MINUTE:
        return "min";
    case AggregationInterval::ONE_HOUR:
        return "hour";
    case AggregationInterval::ONE_DAY:
        return "day";
    }
}

} // namespace sampling
} // namespace bandwit