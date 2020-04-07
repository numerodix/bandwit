#ifndef TERMINAL_SURFACE_H
#define TERMINAL_SURFACE_H

#include <signal.h>

#include "termui/dimensions.h"
#include "termui/point.h"
#include "termui/terminal_driver.h"

namespace bmon {
namespace termui {

class TerminalWindow;

class TermSurface {
  public:
    TermSurface(TerminalWindow *win, uint16_t num_lines);

    void on_startup();
    void on_window_resize(const Dimensions &win_dim_new);

    void clear_surface();
    void put_char(const Point &point, const char &ch);
    void flush();

    const Dimensions &get_size() const;
    const Point &get_upper_left() const;
    const Point &get_lower_right() const;

  private:
    void check_surface_fits(const Dimensions &win_dim);
    Dimensions recompute_dimensions(const Dimensions &win_dim) const;
    Point recompute_lower_right(const Dimensions &win_dim,
                                const Point &upper_left) const;

    TerminalWindow *win_{nullptr};
    uint16_t num_lines_{0};
    const char bg_char_ = ' ';

    Dimensions dim_{};
    Point upper_left_{};
    Point lower_right_{};
};

// eugh
static TerminalWindow *WINDOW = nullptr;

void TerminalWindow_signal_handler(int sig);

class TerminalWindow {
  public:
    static TerminalWindow *create(TerminalDriver *driver) {
        // check WINDOW is nullptr
        WINDOW = new TerminalWindow(driver);
        return WINDOW;
    }

    ~TerminalWindow() { WINDOW = nullptr; }

    void on_resize() {
        auto dim_new = driver_->get_terminal_size();

        if (surface_ != nullptr) {
            surface_->on_window_resize(dim_new);
        }

        dim_ = dim_new;
    }

    const Dimensions &get_size() const { return dim_; }

    const Point &get_cursor() const { return cursor_; }

    void set_cursor(const Point &point) {
        // check point within dimensions
        driver_->set_cursor_position(point);
        cursor_ = point;
    }

    void put_char(const char &ch) {
        // check cursor pos leaves space for the char on the window?
        driver_->put_char(ch);
        // recalculate and update cursor_ ? (is_printable etc)
    }

    void flush() { driver_->flush_output(); }

    void clear_screen(const char &fill_char) {
        auto top_left = Point{1, 1};
        auto dim = get_size();

        set_cursor(top_left);

        for (auto y = 1; y <= dim.height; ++y) {
            for (auto x = 1; x <= dim.width; ++x) {
                put_char(fill_char);
            }
        }

        set_cursor(top_left);
        flush();
    }

    void register_surface(TermSurface *surface) { surface_ = surface; }

  private:
    TerminalWindow(TerminalDriver *driver) : driver_{driver} {
        // the window has to know its size at all times
        dim_ = driver_->get_terminal_size();

        // check cursor within dimensions?
        cursor_ = driver_->get_cursor_position();

        // install handler to update size when it changes
        install_resize_handler();
    }

    void install_resize_handler() {
        signal(SIGWINCH, TerminalWindow_signal_handler);
    }

    TerminalDriver *driver_{nullptr};
    Dimensions dim_{};
    Point cursor_{};
    TermSurface *surface_{nullptr};
};

} // namespace termui
} // namespace bmon

#endif // TERMINAL_SURFACE_H
