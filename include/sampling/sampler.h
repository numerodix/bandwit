#ifndef SAMPLER_H
#define SAMPLER_H

#include <string>

#include "sample.h"

namespace bmon {
namespace sampling {

class Sampler {

  public:
    // we don't actually want this: "abstract classes should not have a ctor",
    // but without it deleting the copy/move ctors below breaks the ctors in the
    // derived classes :(
    Sampler() = default;

    // this is a base so the dtor should be virtual
    virtual ~Sampler() = default;

    // disable copy/move constructors
    Sampler(const Sampler &) = delete;
    Sampler(Sampler &&) = delete;

    // disable assignment
    Sampler &operator=(const Sampler &) = delete;
    Sampler &operator=(Sampler &&) = delete;

    virtual Sample get_sample(const std::string &iface_name) const = 0;
};

} // namespace sampling
} // namespace bmon

#endif // SAMPLER_H
