#include <signal.h>

#include <stdexcept>
#include "signals.h"


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

} // namespace termui
} // namespace bmon