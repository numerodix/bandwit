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