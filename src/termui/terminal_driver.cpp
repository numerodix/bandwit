#include <stdexcept>
#include <sys/ioctl.h>

#include "terminal_driver.h"

namespace bmon {
namespace termui {

Dimensions TerminalDriver::get_terminal_size() {
    struct winsize size {};
    int stdout_fileno = fileno(stdout_file_);

    if (ioctl(stdout_fileno, TIOCGWINSZ, &size) < 0) {
        throw std::runtime_error(
            "TerminalDriver.get_terminal_size failed in ioctl()");
    }

    Dimensions dim{size.ws_col, size.ws_row};
    return dim;
}

Point TerminalDriver::get_cursor_position() {
    // TODO: document the mode the terminal has to be in for this to work

    // This is bit error prone: store the cursor position in the program to
    // avoid reading it more than just at the very beginning. Maybe on
    // startup just fall back on taking over the whole screen (or retry?).
    fprintf(stdout_file_, "\033[6n");

    int cur_x, cur_y;
    if (fscanf(stdin_file_, "\033[%d;%dR", &cur_y, &cur_x) < 2) {
        throw std::runtime_error(
            "TerminalDriver.get_cursor_position failed in scanf()");
    }

    Point pt{U16(cur_x), U16(cur_y)};
    return pt;
}

void TerminalDriver::set_cursor_position(const Point &pt) {
    fprintf(stdout_file_, "\033[%d;%dH", pt.y, pt.x);
}

void TerminalDriver::put_char(const char &ch) {
    char_str_[0] = ch;
    fprintf(stdout_file_, "%s", char_str_);
}

void TerminalDriver::flush_output() { fflush(stdout_file_); }

} // namespace termui
} // namespace bmon