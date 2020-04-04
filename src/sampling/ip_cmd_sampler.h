#ifndef IP_CMD_SAMPLER_H
#define IP_CMD_SAMPLER_H

#include "sampling/sampler.h"

class IpCommandSampler : public Sampler {

  public:
    IpCommandSampler() = default;
    ~IpCommandSampler() override = default;

    // disable copy/move constructors
    IpCommandSampler(const IpCommandSampler &) = delete;
    IpCommandSampler(IpCommandSampler &&) = delete;

    // disable assignment
    IpCommandSampler &operator=(const IpCommandSampler &) = delete;
    IpCommandSampler &operator=(IpCommandSampler &&) = delete;

    Sample get_sample(const std::string &iface_name) const override;
};

#endif // IP_CMD_SAMPLER_H