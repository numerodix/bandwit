#include <signal.h>
#include <stdexcept>
#include <vector>

#include "termui/dimensions.h"

class SignalSuspender {
  public:
    explicit SignalSuspender(std::initializer_list<int> signums)
        : signums_{signums} {}

    void suspend() {
        sigset_t mask;
        sigemptyset(&mask);

        for (auto signo : signums_) {
            sigaddset(&mask, signo);
        }

        // NOTE: not thread safe
        if (sigprocmask(SIG_BLOCK, &mask, nullptr) < 0) {
            throw std::runtime_error(
                "SignalSuspender.suspend failed in sigprocmask()");
        }
    }

    void restore() {
        sigset_t mask;
        sigemptyset(&mask);

        for (auto signo : signums_) {
            sigaddset(&mask, signo);
        }

        if (sigprocmask(SIG_UNBLOCK, &mask, nullptr) < 0) {
            throw std::runtime_error(
                "SignalSuspender.restore failed in sigprocmask()");
        }
    }

  private:
    std::vector<int> signums_{};
};

class SignalGuard {
  public:
    explicit SignalGuard(SignalSuspender *suspender) : suspender_{suspender} {
        suspender_->suspend();
    }
    ~SignalGuard() { suspender_->restore(); }

  private:
    SignalSuspender *suspender_{nullptr};
};

#include <termios.h>

class TerminalModeSetter {
  public:
    explicit TerminalModeSetter(tcflag_t local_off,
                                SignalSuspender *signal_suspender)
        : local_off_{local_off}, signal_suspender_{signal_suspender} {}

    void set() {
        SignalGuard guard{signal_suspender_};

        struct termios tm {};

        if (tcgetattr(STDIN_FILENO, &tm) < 0) {
            throw std::runtime_error(
                "TerminalModeSetter.set failed in tcgetattr()");
        }

        // save the unmodified state so we can restore it
        orig_termios_ = tm;

        tm.c_lflag &= ~local_off_;

        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tm) < 0) {
            throw std::runtime_error(
                "TerminalModeSetter.set failed in tcsetattr()");
        }

        // Now check that the set actually set all of our flags

        struct termios tm_after {};
        if (tcgetattr(STDIN_FILENO, &tm_after) < 0) {
            throw std::runtime_error(
                "TerminalModeSetter.set failed in #2 tcgetattr()");
        }

        if ((tm_after.c_lflag & local_off_) > 0) {
            throw std::runtime_error(
                "TerminalModeSetter.set failed to actually set the flags!");
        }
    }

    void unset() {
        SignalGuard guard{signal_suspender_};

        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios_) < 0) {
            throw std::runtime_error(
                "TerminalModeSetter.unset failed in tcsetattr()");
        }

        // Now check that the set actually unset all of our flags

        struct termios tm_after {};
        if (tcgetattr(STDIN_FILENO, &tm_after) < 0) {
            throw std::runtime_error(
                "TerminalModeSetter.unset failed in tcgetattr()");
        }

        if ((tm_after.c_lflag & local_off_) != local_off_) {
            throw std::runtime_error(
                "TerminalModeSetter.unset failed to actually unset the flags!");
        }
    }

  private:
    tcflag_t local_off_{};
    struct termios orig_termios_ {};

    SignalSuspender *signal_suspender_{nullptr};
};

class TerminalModeSet {
  public:
    TerminalModeSet &local_on(tcflag_t flag) {
        flags_local_on_ |= flag;
        return *this;
    }

    TerminalModeSet &local_off(tcflag_t flag) {
        flags_local_off_ |= flag;
        return *this;
    }

    TerminalModeSetter build_setter(SignalSuspender *signal_suspender) {
        TerminalModeSetter setter{flags_local_off_, signal_suspender};
        return setter;
    }

  private:
    tcflag_t flags_local_on_{};
    tcflag_t flags_local_off_{};
};

class TerminalModeGuard {
  public:
    explicit TerminalModeGuard(TerminalModeSetter *setter) : setter_{setter} {
        setter_->set();
    }
    ~TerminalModeGuard() { setter_->unset(); }

  private:
    TerminalModeSetter *setter_{nullptr};
};

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

// XXXXXXX

class TerminalWindow;

class TermSurface {
  public:
    TermSurface(TerminalWindow *win, uint16_t num_lines);
    void on_window_resize();
    void redraw();

  private:
    TerminalWindow *win_{nullptr};
    uint16_t num_lines_{0};
    const char bg_char_ = 'X';
};

// eugh
static TerminalWindow *WINDOW = nullptr;

void TerminalWindow_signal_handler(int sig);

class TerminalWindow {
  public:
    static TerminalWindow *create(TerminalDriver *driver) {
        // check WINDOW is nullptr
        WINDOW = new TerminalWindow(driver);
        return WINDOW;
    }

    ~TerminalWindow() { WINDOW = nullptr; }

    void on_resize() {
        dim_ = driver_->get_terminal_size();

        for (auto surface : surfaces_) {
            surface->on_window_resize();
        }

        // clear_screen('X'); /// XXX
        // std::cout << "[dim] cols: " << dim_.width << ", rows: " <<
        // dim_.height
        //           << "\n";
    }

    const Dimensions &get_size() const { return dim_; }

    const Point &get_cursor() const { return cursor_; }

    void set_cursor(const Point &point) {
        // check point within dimensions
        driver_->set_cursor_position(point);
        cursor_ = point;
    }

    void put_char(const char &ch) {
        // check cursor pos leaves space for the char on the window?
        driver_->put_char(ch);
        // recalculate and update cursor_ ? (is_printable etc)
    }

    void flush() { driver_->flush_output(); }

    void clear_screen(const char &fill_char) {
        auto top_left = Point{1, 1};
        auto dim = get_size();

        set_cursor(top_left);

        for (auto y = 1; y <= dim.height; ++y) {
            for (auto x = 1; x <= dim.width; ++x) {
                put_char(fill_char);
            }
        }

        set_cursor(top_left);
        flush();
    }

    void register_surface(TermSurface *surface) {
        surfaces_.push_back(surface);
    }

  private:
    TerminalWindow(TerminalDriver *driver) : driver_{driver} {
        // the window has to know its size at all times
        dim_ = driver_->get_terminal_size();

        // check cursor within dimensions?
        cursor_ = driver_->get_cursor_position();

        // install handler to update size when it changes
        install_resize_handler();
    }

    void install_resize_handler() {
        signal(SIGWINCH, TerminalWindow_signal_handler);
    }

    TerminalDriver *driver_{nullptr};
    Dimensions dim_{};
    Point cursor_{};
    std::vector<TermSurface *> surfaces_{};
};

void TerminalWindow_signal_handler(int sig) {
    // check WINDOW is not nullptr
    WINDOW->on_resize();
}

TermSurface::TermSurface(TerminalWindow *win, uint16_t num_lines)
    : win_{win}, num_lines_{num_lines} {
    win_->register_surface(this);
}

void TermSurface::on_window_resize() {
    // After a redraw() the cursor is in the lower right of the surface.
    // We need to move it to the surface upper left before calling redraw() again.

    // the post resize dimensions
    auto dim = win_->get_size();
    // the cursor position has not changed
    auto cur = win_->get_cursor();

    // Case 1: After resize the terminal is too small to display the surface
    //      Hard error.
    // Case 2: After resize the lower right hand corner is below the bottom edge
    // of the terminal.
    //      We cannot redraw without clobbering text that used to be above
    //      the surface, so we might as well clear the whole screen and
    //      move the surface to the top. Then move the cursor to the top left
    //      hand side of the new surface location.
    // Case default: Just move the cursor to the upper left hand side of the
    // surface.

    auto surface_upper_left_x = U16(1);
    auto surface_upper_left_y = U16(1);
    char clear_fill_char = ' ';

    // Resize made the window is too small for surface
    if (num_lines_ > dim.height) {
        win_->clear_screen(clear_fill_char);
        // seems to leave the terminal in cbreak mode :/
        throw std::runtime_error("terminal window too small :(");

    // Resize decreased height and shifted cursor to below the bottom edge
    } else if (cur.y > dim.height) {
        win_->clear_screen(clear_fill_char);

    // Any other resize
    } else {
        surface_upper_left_y = U16(cur.y - num_lines_ + 1);
    }

    auto surface_upper_left = Point{surface_upper_left_x, surface_upper_left_y};
    win_->set_cursor(surface_upper_left);

    redraw();
}

void TermSurface::redraw() {
    auto dim = win_->get_size();
    auto cur = win_->get_cursor();

    auto win_top_left = Point{1, 1};
    auto win_lower_right = Point{dim.width, dim.height};

    auto surface_lower_right_y = std::min(U16(cur.y + num_lines_ - 1), dim.height);
    auto surface_lower_right = Point{dim.width, surface_lower_right_y};

    for (int y = 0; y < num_lines_; ++y) {
        for (int x = 1; x <= dim.width; ++x) {
            win_->put_char(bg_char_);
        }
    }

    win_->set_cursor(win_top_left);
    win_->put_char('Y');
    win_->put_char('Y');
    win_->put_char('Y');

    win_->set_cursor(win_lower_right);
    win_->put_char('T');

    win_->set_cursor(surface_lower_right);
    win_->flush();

    // auto [cols, rows] = get_term_size();
    // auto [cur_x, cur_y] = get_cursor_pos();

    // for (int y = 0; y < num_lines; ++y) {
    //     for (int x = 0; x < cols; ++x) {
    //         fprintf(stdout, "%d", y);
    //     }
    // }

    // fprintf(stdout, "\033[%d;%dH", 1, 1);
    // fprintf(stdout, "YYY");

    // fprintf(stdout, "\033[%d;%dH", rows, cols);
    // fprintf(stdout, "T");

    // int ypos = std::min(U16(num_lines + cur_y - 1), rows);
    // fprintf(stdout, "\033[%d;%dH", ypos, cols);
    // fflush(stdout);
}

} // namespace termui
} // namespace bmon