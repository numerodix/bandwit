#ifndef AGG_INTERVAL_H
#define AGG_INTERVAL_H

#include <string>

namespace bandwit {
namespace sampling {

enum class AggregationInterval {
    ONE_SECOND = 1,
    ONE_MINUTE = 60,
    ONE_HOUR = 3600,
    ONE_DAY = 86400,
};

AggregationInterval next_interval(AggregationInterval agg_interval);
std::string get_label(AggregationInterval agg_interval);

} // namespace sampling
} // namespace bandwit

#endif // AGG_INTERVAL_H
