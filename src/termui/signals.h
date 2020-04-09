#ifndef SIGNALS_H
#define SIGNALS_H

#include <initializer_list>
#include <vector>

namespace bmon {
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

  private:
    SignalSuspender *suspender_{nullptr};
};

} // namespace termui
} // namespace bmon

#endif // SIGNALS_H
