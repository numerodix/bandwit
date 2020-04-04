#include <chrono>
#include <fstream>
#include <regex>
#include <stdexcept>
#include <vector>

#include "procfs_sampler.h"

namespace bmon {
namespace sampling {

class ProcFsParser {
  public:
    std::vector<std::string> read_file_as_lines() const {
        std::ifstream fl{filepath_};
        if (!fl) {
            throw std::runtime_error("failed to open file for reading");
        }

        std::string line{};
        std::vector<std::string> lines{};

        while (getline(fl, line)) {
            lines.push_back(line);
        }

        return lines;
    }

    std::pair<uint64_t, uint64_t> parse(const std::vector<std::string> &lines,
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

        throw std::runtime_error(
            "failed to find the right iface / parse output");
    }

  private:
    std::string filepath_{"/proc/net/dev"};
    std::regex pat_line_{
        R"(^\s*([A-Za-z0-9]+):\s+([0-9]+)\s+([0-9]+)\s+([0-9]+)\s+([0-9]+)\s+([0-9]+)\s+([0-9]+)\s+([0-9]+)\s+([0-9]+)\s+([0-9]+))"};
};

Sample ProcFsSampler::get_sample(const std::string &iface_name) const {
    ProcFsParser parser{};

    auto tp = std::chrono::system_clock::now();
    std::time_t ts = std::chrono::system_clock::to_time_t(tp);

    auto lines = parser.read_file_as_lines();
    auto pair = parser.parse(lines, iface_name);

    Sample sample{
        pair.first,
        pair.second,
        ts,
    };

    return sample;
}

} // namespace sampling
} // namespace bmon
