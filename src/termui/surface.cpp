#include <sstream>
#include <stdexcept>

#include "surface.h"

namespace bmon {
namespace termui {

void Surface::draw() {
    reset_cursor_to_origin();
    clear_screen('-');
}

void Surface::reset_cursor_to_origin() {
    int x = static_cast<int>(origin_.x);
    int y = static_cast<int>(origin_.y);
    fprintf(stdout, "\033[%d;%dH", y, x);
}

void Surface::clear_screen(const char fill_char) {
    std::stringstream ss{};

    for (auto y = 0; y < height_; ++y) {
        for (auto x = 0; x < width_; ++x) {
            ss << fill_char;
        }
        if (y != height_ - 1) {
            ss << '\n';
        }
    }

    std::string block = ss.str();
    fprintf(stdout, block.c_str());
    fflush(stdout);
}

} // namespace termui
} // namespace bmon
