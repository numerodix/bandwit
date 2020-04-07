#ifndef POINT_H
#define POINT_H

#include <cstdint>

namespace bmon {
namespace termui {

struct Point {
    uint16_t x;
    uint16_t y;

    // TODO: add inc_x(int), int_y(int) functions that add/sub a delta
    // while checking for underflow/overflow
};

} // namespace termui
} // namespace bmon

#endif // POINT_H
