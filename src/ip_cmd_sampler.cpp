#include <assert.h>

#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "ip_cmd_sampler.h"


class ProgramRunner {
public:
    std::string run(const std::string& args) const {
        std::array<char, 1024> buffer;
        std::string result;

        FILE *fl = popen(args.c_str(), "r");
        if (!fl) {
            throw std::runtime_error("popen() failed");
        }

        while (fgets(buffer.data(), buffer.size(), fl) != nullptr) {
            result += buffer.data();
        }

        int status_code = pclose(fl);
        if (status_code) {
            throw std::runtime_error("program return non-zero status code");
        }

        return result;
    }
};


class StatsParser {
public:
    std::vector<std::string_view> splitlines(const std::string& output) {
        std::vector<std::string_view> lines;
        std::size_t cursor = 0;

        while (std::size_t pos = output.find("\n", cursor)) {
            if (pos == std::string::npos) {
                break;
            }

            std::string_view line(&output[cursor], pos - cursor);
            lines.push_back(line);
            cursor = pos + 1;
        }

        return lines;
    }

    uint64_t parse_nbytes(const std::string& line) {
        std::smatch mres;
        bool matches = std::regex_search(line, mres, pat_bytes_);
        assert(matches == true);

        std::string bytes_s = mres[1];
        return std::stoul(bytes_s);
    }

    std::pair<uint64_t, uint64_t> parse(const std::string& output, const std::string& iface_name) {
        auto lines = splitlines(output);

        std::string cur_iface{};

        bool next_line_is_rx{false};
        bool next_line_is_tx{false};

        uint64_t rx{0};
        uint64_t tx{0};

        for (const std::string_view& line_view: lines) {
            std::string line(line_view);

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
            }

            // match RX: line
            matches = std::regex_search(line, pat_rx_);
            if (matches) {
                next_line_is_rx = true;
            }

            // match TX: line
            matches = std::regex_search(line, pat_tx_);
            if (matches) {
                next_line_is_tx = true;
            }

            // We've found the right iface and we've parsed rx and tx!
            if ((cur_iface == iface_name) && (rx && tx)) {
                return std::make_pair(rx, tx);
            }
        }

        throw std::runtime_error("failed to find the right iface / parse output");
    }

private:
    std::regex pat_iface_{R"(^([0-9]+): ([A-Za-z0-9]+):)"};
    std::regex pat_rx_{R"(^    RX)"};
    std::regex pat_tx_{R"(^    TX)"};
    std::regex pat_bytes_{R"(^    ([0-9]+))"};
};


Sample IpCommandSampler::get_sample(const std::string& iface_name) const {
    ProgramRunner runner{};
    StatsParser parser{};

    auto tp = std::chrono::system_clock::now();
    std::time_t ts = std::chrono::system_clock::to_time_t(tp);

    std::stringstream ss;
    ss << "ip -statistics link show dev " << iface_name;
    std::string args = ss.str();

    auto output = runner.run(args);
    auto pair = parser.parse(output, iface_name);

    Sample sample{
        .rx = pair.first,
        .tx = pair.second,
        .ts = ts,
    };

    return sample;
}