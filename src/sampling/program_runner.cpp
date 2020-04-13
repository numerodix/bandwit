#include <array>
#include <stdexcept>

#include "program_runner.hpp"

namespace bmon {
namespace sampling {

std::vector<std::string> ProgramRunner::run(const std::string &args) const {
    FILE *fl = popen(args.c_str(), "r");
    if (fl == nullptr) {
        throw std::runtime_error("popen() failed");
    }

    std::array<char, 1024> buffer{};
    std::vector<std::string> lines{};

    while (fgets(buffer.data(), buffer.size(), fl) != nullptr) {
        lines.emplace_back(buffer.data());
    }

    int status_code = pclose(fl);
    if (status_code != 0) {
        throw std::runtime_error("program return non-zero status code");
    }

    return lines;
}

} // namespace sampling
} // namespace bmon