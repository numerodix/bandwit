#ifndef TERMINAL_DRIVER_H
#define TERMINAL_DRIVER_H

#include <iostream>
#include <stdexcept>
#include <sys/ioctl.h>

#include "macros.h"
#include "termui/dimensions.h"

namespace bmon {
namespace termui {

class TerminalDriver {
  public:
    TerminalDriver(FILE *stdin_file, FILE *stdout_file)
        : stdin_file_{stdin_file}, stdout_file_{stdout_file} {}

    Dimensions get_terminal_size() {
        struct winsize size {};
        int stdout_fileno = fileno(stdout_file_);

        if (ioctl(stdout_fileno, TIOCGWINSZ, &size) < 0) {
            throw std::runtime_error(
                "TerminalDriver.get_terminal_size failed in ioctl()");
        }

        Dimensions dim{size.ws_col, size.ws_row};
        return dim;
    }

    Point get_cursor_position() {
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

    void set_cursor_position(const Point &pt) {
        fprintf(stdout_file_, "\033[%d;%dH", pt.y, pt.x);
    }

    void put_char(const char &ch) {
        char_str_[0] = ch;
        fprintf(stdout_file_, "%s", char_str_);
    }

    void flush_output() { fflush(stdout_file_); }

  private:
    FILE *stdin_file_{};
    FILE *stdout_file_{};

    // we need a one char null terminated string
    char char_str_[2] = {0};
};

} // namespace termui
} // namespace bmon

#endif // TERMINAL_DRIVER_H
