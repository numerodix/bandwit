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

    void clear_screen();
    void put_char(Point loc, char ch);
    void redraw();

    Point get_origin();

  private:
    void set_cursor_to(Point loc);
    void clear_screen(char fill_char, char border_char);

    Point get_lower_rhs();

    Point origin_{};

    uint16_t width_{0};
    uint16_t height_{0};
};

} // namespace termui
} // namespace bmon

#endif // SURFACE_H
