#pragma once

#include "sampler.h"


class IpCommandSampler : public Sampler {

public:
    IpCommandSampler() {}
    ~IpCommandSampler() {}

    Sample get_sample(const std::string& iface_name) const override;
};