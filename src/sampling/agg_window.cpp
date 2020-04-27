#include "sampling/agg_window.hpp"

namespace bandwit {
namespace sampling {

AggregationWindow next_interval(AggregationWindow agg_window) {
    switch (agg_window) {
    case AggregationWindow::ONE_SECOND:
        return AggregationWindow::ONE_MINUTE;
    case AggregationWindow::ONE_MINUTE:
        return AggregationWindow::ONE_HOUR;
    case AggregationWindow::ONE_HOUR:
        return AggregationWindow::ONE_DAY;
    case AggregationWindow::ONE_DAY:
        return AggregationWindow::ONE_DAY;
    }
}

AggregationWindow prev_interval(AggregationWindow agg_window) {
    switch (agg_window) {
    case AggregationWindow::ONE_DAY:
        return AggregationWindow::ONE_HOUR;
    case AggregationWindow::ONE_HOUR:
        return AggregationWindow::ONE_MINUTE;
    case AggregationWindow::ONE_MINUTE:
        return AggregationWindow::ONE_SECOND;
    case AggregationWindow::ONE_SECOND:
        return AggregationWindow::ONE_SECOND;
    }
}

std::string get_label(AggregationWindow agg_window) {
    switch (agg_window) {
    case AggregationWindow::ONE_SECOND:
        return "sec";
    case AggregationWindow::ONE_MINUTE:
        return "min";
    case AggregationWindow::ONE_HOUR:
        return "hour";
    case AggregationWindow::ONE_DAY:
        return "day";
    }
}

} // namespace sampling
} // namespace bandwit