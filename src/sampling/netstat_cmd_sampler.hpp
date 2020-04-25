#ifndef NETSTAT_CMD_SAMPLER_H
#define NETSTAT_CMD_SAMPLER_H

#include <regex>

#include "sampling/program_runner.hpp"
#include "sampling/sampler.hpp"

namespace bandwit {
namespace sampling {

class NetstatStatsParser {
  public:
    std::pair<uint64_t, uint64_t> parse(const std::vector<std::string> &lines,
                                        const std::string &iface_name) const;

  private:
    std::regex pat_line_{
        R"(^([A-Za-z0-9]+)\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)\s+([0-9+]+)\s+([^ ]+)\s+([^ ]+)\s+([0-9+]+))"};
};

// This is the only method that is known to work on BSD
class NetstatCommandSampler : public Sampler {
  public:
    NetstatCommandSampler() = default;
    ~NetstatCommandSampler() override = default;

    CLASS_DISABLE_COPIES(NetstatCommandSampler)
    CLASS_DISABLE_MOVES(NetstatCommandSampler)

    Sample get_sample(const std::string &iface_name) const override;

  private:
    ProgramRunner runner_{};
    NetstatStatsParser parser_{};
};

} // namespace sampling
} // namespace bandwit

#endif // NETSTAT_CMD_SAMPLER_H
