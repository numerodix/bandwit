#ifndef TERMINAL_WINDOW_H
#define TERMINAL_WINDOW_H

#include "macros.hpp"
#include "termui/dimensions.hpp"
#include "termui/point.hpp"
#include "termui/window_resize.hpp"

namespace bandwit {
namespace termui {

class SignalSuspender;
class TerminalDriver;

class TerminalWindow {
  public:
    // returns a non-owning pointer because the instance is owned by a static
    // unique_pointer
    static TerminalWindow *create(TerminalDriver *driver,
                                  SignalSuspender *signal_suspender);

    TerminalWindow(TerminalDriver *driver, SignalSuspender *signal_suspender);
    ~TerminalWindow();

    CLASS_DISABLE_COPIES(TerminalWindow)
    CLASS_DISABLE_MOVES(TerminalWindow)

    void on_resize();

    const Dimensions &get_size() const;
    const Point &get_cursor() const;
    void set_cursor(const Point &point);
    void put_char(const char &ch);
    void put_uchar(const std::string &ch);
    void put_string(const std::string &str);
    void flush();
    void clear_screen(const char &fill_char);

    void register_resize_receiver(WindowResizeReceiver *receiver);

  private:
    void check_is_on_window(const Point &point);

    void install_resize_handler();

    TerminalDriver *driver_{nullptr};
    Dimensions dim_{};
    Point cursor_{};
    WindowResizeReceiver *resize_receiver_{nullptr};
    SignalSuspender *signal_suspender_{nullptr};
};

} // namespace termui
} // namespace bandwit

#endif // TERMINAL_WINDOW_H
