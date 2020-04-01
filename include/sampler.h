#pragma once

#include <string>

#include "sample.h"


class Sampler {
    Sample virtual get_sample(const std::string& iface_name) const = 0;
};