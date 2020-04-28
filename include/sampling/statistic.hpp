#ifndef STATISTIC_H
#define STATISTIC_H

#include <string>

namespace bandwit {
namespace sampling {

enum class Statistic {
    AVERAGE,
    SUM,
};

std::string get_label(Statistic stat);

} // namespace sampling
} // namespace bandwit

#endif // STATISTIC_H
