#include <iostream>
#include <stdexcept>
#include <sys/ioctl.h>
#include <unistd.h>

#include "display.h"

namespace bmon {
namespace termui {

Display::~Display() {
    if (terminal_mode_ != TerminalMode::ORIGINAL) {
        reset_term_mode();
    }
}

void Display::initialize() {
    enter_cbreak_mode();
    recreate_surface();
}

void Display::recreate_surface() {
    detect_terminal_size();
    Point origin = detect_terminal_cursor();

    uint16_t max_y = rows_ - num_lines_;
    if (origin.y > max_y) {
        origin.y = max_y;
    }

    auto width = cols_;
    auto height = num_lines_;

    surface_ = std::unique_ptr<Surface>(new Surface(origin, width, height));
    surface_->draw();
}

void Display::enter_cbreak_mode() {
    if (terminal_mode_ == TerminalMode::CBREAK) {
        throw std::runtime_error("called enter_cbreak_mode() in CBREAK mode");
    }

    struct termios tm {};
    if (tcgetattr(STDIN_FILENO, &tm) < 0) {
        throw std::runtime_error("tcgetattr() failed");
    }

    // save original termios so we can restore it later
    stdin_orig_termios_ = tm;

    tm.c_lflag &= ~(ECHO | ICANON);
    // tm.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    // tm.c_cflag &= ~(CSIZE | PARENB);
    // tm.c_oflag &= ~(OPOST);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tm) < 0) {
        throw std::runtime_error("tcsetattr() failed");
    }
    terminal_mode_ = TerminalMode::CBREAK;
}

void Display::reset_term_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &stdin_orig_termios_) < 0) {
        throw std::runtime_error("tcsetattr() failed");
    }
    terminal_mode_ = TerminalMode::ORIGINAL;
}

void Display::detect_terminal_size() {
    struct winsize size {};

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) < 0) {
        throw std::runtime_error(
            "ioctl() failed when trying to read terminal size");
    }

    cols_ = size.ws_col;
    rows_ = size.ws_row;
}

Point Display::detect_terminal_cursor() {
    int x{0};
    int y{0};

    // tell the terminal to print the cursor position
    fprintf(stdout, "\033[6n");

    // read it back from stdin
    if (scanf("\033[%d;%dR", &y, &x) < 2) {
        throw std::runtime_error("failed to read cursor position");
    }

    Point pt{
        // should be okay unless the terminal returns crazy out of range values?
        static_cast<uint16_t>(x),
        static_cast<uint16_t>(y),
    };
    return pt;
}

} // namespace termui
} // namespace bmon