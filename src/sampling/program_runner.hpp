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