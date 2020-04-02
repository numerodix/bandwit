#ifndef SAMPLER_H
#define SAMPLER_H


#include <string>

#include "sample.h"


class Sampler {

public:
    virtual ~Sampler() {}

    // disable copy/move constructors
    // Sampler(const Sampler&) = delete;
    // Sampler(Sampler&&) = delete;

    // disable assignment
    Sampler& operator=(const Sampler&) = delete;
    Sampler& operator=(Sampler&&) = delete;

    virtual Sample get_sample(const std::string& iface_name) const = 0;
};


#endif // SAMPLER_H