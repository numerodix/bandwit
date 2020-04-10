#include <csignal>

#include "signals.hpp"
#include <stdexcept>

namespace bmon {
namespace termui {

void SignalSuspender::suspend() {
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

void SignalSuspender::restore() {
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

void sigint_handler(int sig) {
    // Throw here to force the stack to unwind. If we did just exit() here that
    // would not happen.
    throw termui::InterruptException();
}

} // namespace termui
} // namespace bmon