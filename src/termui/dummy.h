#include <signal.h>
#include <stdexcept>
#include <vector>

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
        flags_local_off_ = flags_local_off_ | flag;
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
        setter->set();
    }
    ~TerminalModeGuard() { setter_->unset(); }

  private:
    TerminalModeSetter *setter_{nullptr};
};