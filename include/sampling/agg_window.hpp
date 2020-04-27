#ifndef AGG_WINDOW_H
#define AGG_WINDOW_H

#include <string>

namespace bandwit {
namespace sampling {

enum class AggregationWindow {
    ONE_SECOND = 1,
    ONE_MINUTE = 60,
    ONE_HOUR = 3600,
    ONE_DAY = 86400,
};

AggregationWindow next_interval(AggregationWindow agg_window);
AggregationWindow prev_interval(AggregationWindow agg_window);
std::string get_label(AggregationWindow agg_window);

} // namespace sampling
} // namespace bandwit

#endif // AGG_WINDOW_H
