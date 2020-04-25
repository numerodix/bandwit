#include <fstream>
#include <stdexcept>

#include "aliases.hpp"
#include "except.hpp"
#include "procfs_sampler.hpp"

namespace bandwit {
namespace sampling {

std::vector<std::string> ProcFsParser::read_file_as_lines() const {
    std::ifstream fl{filepath_};
    if (!fl) {
        THROW_ARGS(std::runtime_error, "failed to open file for reading: %s",
                   filepath_.c_str());
    }

    std::string line{};
    std::vector<std::string> lines{};

    while (getline(fl, line)) {
        lines.push_back(std::move(line));
    }

    return lines;
}

std::pair<uint64_t, uint64_t>
ProcFsParser::parse(const std::vector<std::string> &lines,
                    const std::string &iface_name) const {
    for (const std::string &line : lines) {
        std::smatch mres;

        bool matches = std::regex_search(line, mres, pat_line_);
        if (matches) {
            std::string cur_iface = mres[1];
            if (cur_iface == iface_name) {
                std::string rx_s = mres[2];
                std::string tx_s = mres[10];

                uint64_t rx = std::stoul(rx_s);
                uint64_t tx = std::stoul(tx_s);

                return std::make_pair(rx, tx);
            }
        }
    }

    THROW_MSG(std::runtime_error,
              "failed to find the right iface / parse output");
}

Sample ProcFsSampler::get_sample(const std::string &iface_name) const {
    auto tp = Clock::now();
    std::time_t ts = Clock::to_time_t(tp);

    auto lines = parser_.read_file_as_lines();
    auto pair = parser_.parse(lines, iface_name);

    Sample sample{
        pair.first,
        pair.second,
        ts,
    };

    return sample;
}

} // namespace sampling
} // namespace bandwit
