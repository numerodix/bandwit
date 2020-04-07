#include <signal.h>

#include "terminal_driver.h"
#include "terminal_surface.h"
#include "terminal_window.h"

namespace bmon {
namespace termui {

// eugh
static TerminalWindow *WINDOW = nullptr;

void TerminalWindow_signal_handler(int sig) {
    // check WINDOW is not nullptr
    WINDOW->on_resize();
}

TerminalWindow *TerminalWindow::create(TerminalDriver *driver) {
    // check WINDOW is nullptr
    WINDOW = new TerminalWindow(driver);
    return WINDOW;
}

TerminalWindow::~TerminalWindow() { WINDOW = nullptr; }

void TerminalWindow::on_resize() {
    auto dim_new = driver_->get_terminal_size();

    if (surface_ != nullptr) {
        surface_->on_window_resize(dim_new);
    }

    dim_ = dim_new;
}

const Dimensions &TerminalWindow::get_size() const { return dim_; }

const Point &TerminalWindow::get_cursor() const { return cursor_; }

void TerminalWindow::set_cursor(const Point &point) {
    // check point within dimensions
    driver_->set_cursor_position(point);
    cursor_ = point;
}

void TerminalWindow::put_char(const char &ch) {
    // check cursor pos leaves space for the char on the window?
    driver_->put_char(ch);
    // recalculate and update cursor_ ? (is_printable etc)
}

void TerminalWindow::flush() { driver_->flush_output(); }

void TerminalWindow::clear_screen(const char &fill_char) {
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

void TerminalWindow::register_surface(TermSurface *surface) { surface_ = surface; }

TerminalWindow::TerminalWindow(TerminalDriver *driver) : driver_{driver} {
    // the window has to know its size at all times
    dim_ = driver_->get_terminal_size();

    // check cursor within dimensions?
    cursor_ = driver_->get_cursor_position();

    // install handler to update size when it changes
    install_resize_handler();
}

void TerminalWindow::install_resize_handler() {
    signal(SIGWINCH, TerminalWindow_signal_handler);
}

} // namespace termui
} // namespace bmon