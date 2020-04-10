#ifndef TERMINAL_WINDOW_H
#define TERMINAL_WINDOW_H

#include "termui/dimensions.hpp"
#include "termui/point.hpp"

namespace bmon {
namespace termui {

class TerminalDriver;
class TerminalSurface;

class TerminalWindow {
  public:
    static TerminalWindow *create(TerminalDriver *driver);
    ~TerminalWindow();

    void on_resize();

    const Dimensions &get_size() const;
    const Point &get_cursor() const;
    void set_cursor(const Point &point);
    void put_char(const char &ch);
    void flush();
    void clear_screen(const char &fill_char);

    void register_surface(TerminalSurface *surface);

  private:
    TerminalWindow(TerminalDriver *driver);

    void check_is_on_window(const Point &point);

    void install_resize_handler();

    TerminalDriver *driver_{nullptr};
    Dimensions dim_{};
    Point cursor_{};
    TerminalSurface *surface_{nullptr};
};

} // namespace termui
} // namespace bmon

#endif // TERMINAL_WINDOW_H
