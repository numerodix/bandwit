#ifndef SURFACE_H
#define SURFACE_H

#include <cstdint>

#include "termui/point.h"

namespace bmon {
namespace termui {

class Surface {
  public:
    explicit Surface(Point origin, uint16_t width, uint16_t height)
        : origin_{origin}, width_{width}, height_{height} {}

    void draw();

  private:
    void reset_cursor_to_origin();
    void clear_screen(const char fill_char);

    Point origin_{};

    uint16_t width_{0};
    uint16_t height_{0};
};

} // namespace termui
} // namespace bmon

#endif // SURFACE_H
