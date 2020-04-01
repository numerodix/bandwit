#include "sampler.h"


class IpCommandSampler : public Sampler {

public:
    Sample get_sample(const std::string& iface_name) const override;
};