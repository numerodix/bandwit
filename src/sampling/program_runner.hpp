#ifndef PROGRAM_RUNNER_H
#define PROGRAM_RUNNER_H

#include <string>
#include <vector>

namespace bandwit {
namespace sampling {

class ProgramRunner {
  public:
    std::vector<std::string> run(const std::string &args) const;
};

} // namespace sampling
} // namespace bandwit

#endif // PROGRAM_RUNNER_H
