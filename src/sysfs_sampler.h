#pragma once

#include "sampler.h"


class SysFsSampler : public Sampler {

public:
    explicit SysFsSampler() {}
    ~SysFsSampler() {}

    Sample get_sample(const std::string& iface_name) const override;
};
