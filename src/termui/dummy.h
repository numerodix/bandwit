#include <signal.h>
#include <stdexcept>

class SignalController {
  public:
    explicit SignalController(int signo) : signo_{signo} {}

    void disable() {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, signo_);

        if (sigprocmask(SIG_BLOCK, &mask, nullptr) < 0) {
            throw std::runtime_error(
                "SignalController.disable failed in sigprocmask()");
        }
    }

    void reenable() {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, signo_);

        if (sigprocmask(SIG_UNBLOCK, &mask, nullptr) < 0) {
            throw std::runtime_error(
                "SignalController.reenable failed in sigprocmask()");
        }
    }

  private:
    int signo_{};
};

#include <vector>

class SignalControllerSet {
  public:
    explicit SignalControllerSet(std::initializer_list<SignalController> lst)
        : controllers_{lst} {}

    void disable() {
        for (auto controller : controllers_) {
            controller.disable();
        }
    }

    void reenable() {
        for (auto controller : controllers_) {
            controller.reenable();
        }
    }

  private:
    std::vector<SignalController> controllers_{};
};

class SignalGuard {
  public:
    explicit SignalGuard(SignalControllerSet *set) : set_{set} {
        set_->disable();
    }
    ~SignalGuard() { set_->reenable(); }

  private:
    SignalControllerSet *set_{nullptr};
};

#include <termios.h>

class TerminalModeSetter {
  public:
    explicit TerminalModeSetter(tcflag_t local_off,
                                SignalControllerSet *signal_controllers)
        : local_off_{local_off}, signal_controllers_{signal_controllers} {}

    void set() {
        SignalGuard guard{signal_controllers_};

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
        SignalGuard guard{signal_controllers_};

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

    SignalControllerSet *signal_controllers_{nullptr};
};

class TerminalModeGuard {
  public:
    explicit TerminalModeGuard(TerminalModeSetter *setter) : setter_{setter} {
        setter->set();
    }
    ~TerminalModeGuard() { setter_->unset(); }

  private:
    TerminalModeSetter *setter_{nullptr};
};