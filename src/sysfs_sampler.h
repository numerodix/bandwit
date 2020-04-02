#ifndef SYSFS_SAMPLER_H
#define SYSFS_SAMPLER_H


#include "sampler.h"


class SysFsSampler : public Sampler {

public:
    SysFsSampler() {}
    ~SysFsSampler() {}

    Sample get_sample(const std::string& iface_name) const override;
};


#endif // SYSFS_SAMPLER_H