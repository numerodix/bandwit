#ifndef IP_CMD_SAMPLER_H
#define IP_CMD_SAMPLER_H

#include <regex>
#include <string>

#include "sampling/program_runner.hpp"
#include "sampling/sampler.hpp"

namespace bandwit {
namespace sampling {

class IpStatsParser {
  public:
    uint64_t parse_nbytes(const std::string &line) const;
    std::pair<uint64_t, uint64_t> parse(const std::vector<std::string> &lines,
                                        const std::string &iface_name) const;

  private:
    std::regex pat_iface_{R"(^([0-9]+): ([A-Za-z0-9]+):)"};
    std::regex pat_rx_{R"(^    RX)"};
    std::regex pat_tx_{R"(^    TX)"};
    std::regex pat_bytes_{R"(^    ([0-9]+))"};
};

class IpCommandSampler : public Sampler {
  public:
    IpCommandSampler() = default;
    ~IpCommandSampler() override = default;

    CLASS_DISABLE_COPIES(IpCommandSampler)
    CLASS_DISABLE_MOVES(IpCommandSampler)

    Sample get_sample(const std::string &iface_name) const override;

  private:
    ProgramRunner runner_{};
    IpStatsParser parser_{};
};

} // namespace sampling
} // namespace bandwit

#endif // IP_CMD_SAMPLER_H