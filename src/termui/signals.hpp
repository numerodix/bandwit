#ifndef SIGNALS_H
#define SIGNALS_H

#include <initializer_list>
#include <vector>

#include "macros.hpp"

namespace bandwit {
namespace termui {

class SignalSuspender {
  public:
    SignalSuspender(std::initializer_list<int> signums) : signums_{signums} {}

    void suspend();
    void restore();

  private:
    std::vector<int> signums_{};
};

class SignalGuard {
  public:
    explicit SignalGuard(SignalSuspender *suspender) : suspender_{suspender} {
        suspender_->suspend();
    }
    ~SignalGuard() { suspender_->restore(); }

    CLASS_DISABLE_COPIES(SignalGuard)
    CLASS_DISABLE_MOVES(SignalGuard)

  private:
    SignalSuspender *suspender_{nullptr};
};

void sigint_handler(int sig);

// Intended to be thrown from a signal handler that handles SIGINT
class InterruptException : public std::exception {
  public:
    const char *what() const noexcept override { return "Got signal SIGINT"; }
};

} // namespace termui
} // namespace bandwit

#endif // SIGNALS_H
