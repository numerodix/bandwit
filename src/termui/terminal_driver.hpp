#ifndef TERMINAL_DRIVER_H
#define TERMINAL_DRIVER_H

#include <iostream>

#include "macros.hpp"
#include "termui/dimensions.hpp"
#include "termui/point.hpp"

namespace bandwit {
namespace termui {

class FileStatusSetter;

class TerminalDriver {
  public:
    TerminalDriver(FILE *stdin_file, FILE *stdout_file,
                   FileStatusSetter *status_setter)
        : stdin_file_{stdin_file}, stdout_file_{stdout_file},
          status_setter_{status_setter} {}

    Dimensions get_terminal_size();
    Point get_cursor_position();
    void set_cursor_position(const Point &pt);
    void put_char(const char &ch);
    void put_uchar(const std::string &ch);
    void flush_output();

  private:
    FILE *stdin_file_{};
    FILE *stdout_file_{};

    FileStatusSetter *status_setter_{nullptr};

    // we need a one char null terminated string
    char char_str_[2] = {0};
};

} // namespace termui
} // namespace bandwit

#endif // TERMINAL_DRIVER_H
