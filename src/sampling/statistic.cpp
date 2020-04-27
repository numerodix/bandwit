#include "sampling/statistic.hpp"

namespace bandwit {
namespace sampling {

std::string get_label(Statistic stat) {
    switch (stat) {
    case Statistic::AVERAGE:
        return "avg";
    case Statistic::SUM:
        return "sum";
    }
}

} // namespace sampling
} // namespace bandwit