#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "ip_cmd_sampler.hpp"

namespace bmon {
namespace sampling {

class ProgramRunner {
  public:
    std::vector<std::string> run(const std::string &args) const {
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
};

class StatsParser {
  public:
    uint64_t parse_nbytes(const std::string &line) {
        std::smatch mres{};
        bool matches = std::regex_search(line, mres, pat_bytes_);
        if (!matches) {
            throw std::runtime_error("regex pat_bytes_ failed to match line");
        }

        std::string bytes_s = mres[1];
        return std::stoul(bytes_s);
    }

    std::pair<uint64_t, uint64_t> parse(const std::vector<std::string> &lines,
                                        const std::string &iface_name) {
        std::string cur_iface{};

        bool next_line_is_rx{false};
        bool next_line_is_tx{false};

        int64_t rx{-1};
        int64_t tx{-1};

        for (const std::string &line : lines) {
            // Did we match RX: on the previous line?
            if (next_line_is_rx) {
                rx = parse_nbytes(line);
                next_line_is_rx = false;
            }

            // Did we match TX: on the previous line?
            if (next_line_is_tx) {
                tx = parse_nbytes(line);
                next_line_is_tx = false;
            }

            // match iface name
            std::smatch mres_iface;
            bool matches = std::regex_search(line, mres_iface, pat_iface_);
            if (matches) {
                cur_iface = mres_iface[2];
                continue;
            }

            // match RX: line
            matches = std::regex_search(line, pat_rx_);
            if (matches) {
                next_line_is_rx = true;
                continue;
            }

            // match TX: line
            matches = std::regex_search(line, pat_tx_);
            if (matches) {
                next_line_is_tx = true;
                continue;
            }

            // We've found the right iface and we've parsed rx and tx!
            if ((cur_iface == iface_name) && (rx >= 0 && tx >= 0)) {
                auto urx = U64(rx);
                auto utx = U64(tx);
                return std::make_pair(urx, utx);
            }
        }

        throw std::runtime_error(
            "failed to find the right iface / parse output");
    }

  private:
    std::regex pat_iface_{R"(^([0-9]+): ([A-Za-z0-9]+):)"};
    std::regex pat_rx_{R"(^    RX)"};
    std::regex pat_tx_{R"(^    TX)"};
    std::regex pat_bytes_{R"(^    ([0-9]+))"};
};

Sample IpCommandSampler::get_sample(const std::string &iface_name) const {
    ProgramRunner runner{};
    StatsParser parser{};

    auto tp = std::chrono::system_clock::now();
    std::time_t ts = std::chrono::system_clock::to_time_t(tp);

    std::stringstream ss{};
    ss << "ip -statistics link show dev " << iface_name;
    std::string args = ss.str();

    auto output = runner.run(args);
    auto pair = parser.parse(output, iface_name);

    Sample sample{
        pair.first,
        pair.second,
        ts,
    };

    return sample;
}

} // namespace sampling
} // namespace bmon