#pragma once

#include <string>

#include "sample.h"


class Sampler {

public:
    virtual ~Sampler() {}

    // Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;

    // Sampler(Sampler&&) = delete;
    Sampler& operator=(Sampler&&) = delete;

    Sample virtual get_sample(const std::string& iface_name) const = 0;
};