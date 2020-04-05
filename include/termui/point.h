#ifndef POINT_H
#define POINT_H

#include <cstdint>

namespace bmon {
namespace termui {

struct Point {
    uint16_t x;
    uint16_t y;
};

} // namespace termui
} // namespace bmon

#endif // POINT_H
