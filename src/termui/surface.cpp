#include <sstream>
#include <stdexcept>

#include "macros.h"
#include "surface.h"

namespace bmon {
namespace termui {

void Surface::clear_screen() {
    set_cursor_to(origin_);
    clear_screen(' ', ' ');
}

void Surface::put_char(Point loc, char ch) {
    auto x = INT(loc.x);
    auto y = INT(loc.y);
    fprintf(stdout, "\033[%d;%dH", y, x);
    fputc(ch, stdout);
}

void Surface::redraw() {
    Point end = get_lower_rhs();
    set_cursor_to(end);
}

Point Surface::get_origin() { return origin_; }

void Surface::set_cursor_to(Point loc) {
    auto x = INT(loc.x);
    auto y = INT(loc.y);
    fprintf(stdout, "\033[%d;%dH", y, x);
    fflush(stdout);
}

void Surface::clear_screen(const char fill_char, const char border_char) {
    std::stringstream ss{};

    for (auto y = 0; y < height_; ++y) {
        for (auto x = 0; x < width_; ++x) {
            if ((x == 0) || (x == width_ - 1)) {
                ss << border_char;
            } else if ((y == 0) || (y == height_ - 1)) {
                ss << border_char;
            } else {
                ss << fill_char;
            }
        }
        if (y != height_ - 1) {
            ss << '\n';
        }
    }

    std::string block = ss.str();
    fprintf(stdout, "%s", block.c_str());
    fflush(stdout);
}

Point Surface::get_lower_rhs() {
    uint16_t x = origin_.x + width_;
    uint16_t y = origin_.y + height_;

    Point pt{
        x,
        y,
    };

    return pt;
}

} // namespace termui
} // namespace bmon