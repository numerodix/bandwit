#ifndef PROCFS_SAMPLER_H
#define PROCFS_SAMPLER_H

#include <regex>
#include <string>

#include "sampling/sampler.hpp"

namespace bandwit {
namespace sampling {

class ProcFsParser {
  public:
    std::vector<std::string> read_file_as_lines() const;
    std::pair<uint64_t, uint64_t> parse(const std::vector<std::string> &lines,
                                        const std::string &iface_name) const;

  private:
    std::string filepath_{"/proc/net/dev"};
    std::regex pat_line_{
        R"(^\s*([A-Za-z0-9]+):\s+([0-9]+)\s+([0-9]+)\s+([0-9]+)\s+([0-9]+)\s+([0-9]+)\s+([0-9]+)\s+([0-9]+)\s+([0-9]+)\s+([0-9]+))"};
};

class ProcFsSampler : public Sampler {
  public:
    ProcFsSampler() = default;
    ~ProcFsSampler() override = default;

    CLASS_DISABLE_COPIES(ProcFsSampler)
    CLASS_DISABLE_MOVES(ProcFsSampler)

    Sample get_sample(const std::string &iface_name) const override;

  private:
    ProcFsParser parser_{};
};

} // namespace sampling
} // namespace bandwit

#endif // PROCFS_SAMPLER_H