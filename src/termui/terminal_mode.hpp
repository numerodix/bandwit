#ifndef TERMINAL_MODE_H
#define TERMINAL_MODE_H

#include <termios.h>

namespace bmon {
namespace termui {

class SignalSuspender;

class TerminalModeSetter {
  public:
    explicit TerminalModeSetter(tcflag_t local_off,
                                SignalSuspender *signal_suspender)
        : local_off_{local_off}, signal_suspender_{signal_suspender} {}

    void set();
    void unset();

  private:
    tcflag_t local_off_{};
    struct termios orig_termios_ {};

    SignalSuspender *signal_suspender_{nullptr};
};

class TerminalModeSet {
  public:
    TerminalModeSet &local_on(tcflag_t flag);
    TerminalModeSet &local_off(tcflag_t flag);
    TerminalModeSetter build_setter(SignalSuspender *signal_suspender);

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

} // namespace termui
} // namespace bmon

#endif // TERMINAL_MODE_H