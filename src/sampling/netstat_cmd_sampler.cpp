#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "aliases.hpp"
#include "netstat_cmd_sampler.hpp"

namespace bandwit {
namespace sampling {

std::pair<uint64_t, uint64_t>
NetstatStatsParser::parse(const std::vector<std::string> &lines,
                          const std::string &iface_name) const {
    std::string cur_iface{};

    int64_t rx{-1};
    int64_t tx{-1};

    for (const std::string &line : lines) {
        std::smatch mres_lines;
        bool matches = std::regex_search(line, mres_lines, pat_line_);
        if (matches) {
            cur_iface = mres_lines[1];

            if (cur_iface == iface_name) {
                std::string rx_s = mres_lines[8];
                rx = std::stoul(rx_s);

                std::string tx_s = mres_lines[11];
                tx = std::stoul(tx_s);

                auto urx = U64(rx);
                auto utx = U64(tx);
                return std::make_pair(urx, utx);
            }
        }
    }

    throw std::runtime_error(
        "failed to find the right iface / parse output");
}

Sample NetstatCommandSampler::get_sample(const std::string &iface_name) const {
    auto tp = Clock::now();
    std::time_t ts = Clock::to_time_t(tp);

    std::string args{"netstat -ibn"};

    auto output = runner_.run(args);
    auto pair = parser_.parse(output, iface_name);

    Sample sample{
        pair.first,
        pair.second,
        ts,
    };

    return sample;
}

} // namespace sampling
} // namespace bandwit
