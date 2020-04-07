#ifndef TERMINAL_DRIVER_H
#define TERMINAL_DRIVER_H

#include <iostream>

#include "macros.h"
#include "termui/dimensions.h"
#include "termui/point.h"

namespace bmon {
namespace termui {

class TerminalDriver {
  public:
    TerminalDriver(FILE *stdin_file, FILE *stdout_file)
        : stdin_file_{stdin_file}, stdout_file_{stdout_file} {}

    Dimensions get_terminal_size();
    Point get_cursor_position();
    void set_cursor_position(const Point &pt);
    void put_char(const char &ch);
    void flush_output();

  private:
    FILE *stdin_file_{};
    FILE *stdout_file_{};

    // we need a one char null terminated string
    char char_str_[2] = {0};
};

} // namespace termui
} // namespace bmon

#endif // TERMINAL_DRIVER_H
