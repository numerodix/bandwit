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
            throw std::runtime_error("SignalController.disable failed in sigprocmask()");
        }
    }

    void enable() {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, signo_);

        if (sigprocmask(SIG_UNBLOCK, &mask, nullptr) < 0) {
            throw std::runtime_error("SignalController.enable failed in sigprocmask()");
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

    void enable() const {
        for (auto controller: controllers_) {
            controller.enable();
        }
    }

    void disable() const {
        for (auto controller: controllers_) {
            controller.disable();
        }
    }

  private:
    std::vector<SignalController> controllers_{};
};


#include <termios.h>

class TerminalModeSetter {
  public:
    TerminalModeSetter(tcflag_t local_off, const SignalControllerSet& signal_controllers)
     : local_off_{local_off}, signal_controllers_{signal_controllers} {}

    void set() {
        signal_controllers_.disable();

        struct termios tm{};

        if (tcgetattr(STDIN_FILENO, &tm) < 0) {
            throw std::runtime_error("TerminalModeSetter.enable failed in tcgetattr()");
        }

        tm.c_lflag &= ~(local_off_);

        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tm) < 0) {
            throw std::runtime_error("TerminalModeSetter.enable failed in tcsetattr()");
        }

        signal_controllers_.enable();
    }

  private:
    tcflag_t local_off_{};
    const SignalControllerSet& signal_controllers_{};
};