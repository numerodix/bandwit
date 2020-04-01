#pragma once

#include "sampler.h"


class IpCommandSampler : public Sampler {

public:
    explicit IpCommandSampler() {}
    ~IpCommandSampler() {}

    Sample get_sample(const std::string& iface_name) const override;
};