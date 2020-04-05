#include <iostream>
#include <stdexcept>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "display.h"

namespace bmon {
namespace termui {

void Display::initialize() { detect_size(); }

void Display::detect_size() {
    struct winsize size {};

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, (char *)&size) < 0) {
        throw std::runtime_error("ioctl() failed when trying to read terminal size");
    }

    cols_ = size.ws_col;
    rows_ = size.ws_row;
}

} // namespace termui
} // namespace bmon