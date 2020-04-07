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

    void put_char(char ch) {
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


void TerminalWindow_signal_handler(int sig);

class TerminalWindow;
// eugh
static TerminalWindow *WINDOW = nullptr;

class TerminalWindow {
  public:
    static TerminalWindow* create(TerminalDriver *driver) {
        WINDOW = new TerminalWindow(driver);
        return WINDOW;
    }

    void on_resize() {
        dim_ = driver_->get_terminal_size();
        std::cout << "[dim] cols: " << dim_.width << ", rows: " << dim_.height
                << "\n";
    }

  private:
    TerminalWindow(TerminalDriver *driver) : driver_{driver} {
        on_resize();
        install_resize_handler();
    }

    ~TerminalWindow() {
        WINDOW = nullptr;
    }

    void install_resize_handler() {
        signal(SIGWINCH, TerminalWindow_signal_handler);
    }

    TerminalDriver *driver_{nullptr};
    Dimensions dim_{};
};

void TerminalWindow_signal_handler(int sig) {
    // check WINDOW is not nullptr
    WINDOW->on_resize();
}

} // namespace termui
} // namespace bmon