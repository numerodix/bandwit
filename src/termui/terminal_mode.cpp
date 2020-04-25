#include <stdexcept>
#include <unistd.h>

#include "signals.hpp"
#include "terminal_mode.hpp"

namespace bandwit {
namespace termui {

void TerminalModeSetter::set() {
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

void TerminalModeSetter::reset() {
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

TerminalModeSet &TerminalModeSet::local_off(tcflag_t flag) {
    flags_local_off_ |= flag;
    return *this;
}

TerminalModeSetter
TerminalModeSet::build_setter(SignalSuspender *signal_suspender) {
    TerminalModeSetter setter{flags_local_off_, signal_suspender};
    return setter;
}

std::unique_ptr<TerminalModeSetter>
TerminalModeSet::build_setterp(SignalSuspender *signal_suspender) {
    return std::make_unique<TerminalModeSetter>(flags_local_off_,
                                                signal_suspender);
}

} // namespace termui
} // namespace bandwit