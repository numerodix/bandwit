#include <stdexcept>
#include <sys/ioctl.h>

#include "file_status.hpp"
#include "terminal_driver.hpp"

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
    // For this to work stdin needs to be in blocking mode.
    FileStatusGuard guard{status_setter_};

    // Reading the cursor position has been known to be flaky. To work around
    // this we do it only once on program startup, and then we aim to compute it
    // based on the events we generate (moving the cursor) or the events we
    // observe (window resizing).
    // If we did fail to read the cursor pos on startup we could fall back on
    // taking over the whole terminal window...
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

void TerminalDriver::put_uchar(const std::string &ch) {
    // We can't really validate ch by checking the length or anything, it can be
    // any sequence of bytes that make up a char. It's supposed to be only one
    // char.
    fprintf(stdout_file_, "%s", ch.c_str());
}

void TerminalDriver::flush_output() { fflush(stdout_file_); }

} // namespace termui
} // namespace bmon