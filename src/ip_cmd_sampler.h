#ifndef IP_CMD_SAMPLER_H
#define IP_CMD_SAMPLER_H


#include "sampler.h"


class IpCommandSampler : public Sampler {

public:
    IpCommandSampler() {}
    ~IpCommandSampler() {}

    Sample get_sample(const std::string& iface_name) const override;
};


#endif // IP_CMD_SAMPLER_H