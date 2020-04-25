#include <array>
#include <sstream>
#include <stdexcept>

#include "except.hpp"
#include "program_runner.hpp"

namespace bandwit {
namespace sampling {

std::vector<std::string> ProgramRunner::run(const std::string &args) const {
    // append 2>/dev/null to get rid of stderr output
    std::stringstream ss{};
    ss << args << " 2>/dev/null";
    std::string args_actual = ss.str();

    FILE *fl = popen(args_actual.c_str(), "r");
    if (fl == nullptr) {
        THROW_CERROR(std::runtime_error, "popen() failed");
    }

    std::array<char, 1024> buffer{};
    std::vector<std::string> lines{};

    while (fgets(buffer.data(), buffer.size(), fl) != nullptr) {
        lines.emplace_back(buffer.data());
    }

    int status_code = pclose(fl);
    if (status_code != 0) {
        THROW_CERROR(std::runtime_error,
                     "program returned non-zero status code");
    }

    return lines;
}

} // namespace sampling
} // namespace bandwit