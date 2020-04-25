#include <csignal>
#include <memory>

#include "signals.hpp"
#include "terminal_driver.hpp"
#include "terminal_surface.hpp"
#include "terminal_window.hpp"

namespace bandwit {
namespace termui {

// This is what actually owns the TerminalWindow. There can only be one
// TerminalWindow.
static std::unique_ptr<TerminalWindow> WINDOW = nullptr;

void TerminalWindow_signal_handler([[maybe_unused]] int sig) {
    if (WINDOW == nullptr) {
        throw std::runtime_error("No TerminalWindow exists!");
    }

    WINDOW->on_resize();
}

TerminalWindow *TerminalWindow::create(TerminalDriver *driver,
                                       SignalSuspender *signal_suspender) {
    if (WINDOW != nullptr) {
        throw std::runtime_error("Cannot construct another TerminalWindow!");
    }

    WINDOW = std::make_unique<TerminalWindow>(driver, signal_suspender);
    return WINDOW.get();
}

TerminalWindow::TerminalWindow(TerminalDriver *driver,
                               SignalSuspender *signal_suspender)
    : driver_{driver}, signal_suspender_{signal_suspender} {
    // the window has to know its size at all times
    dim_ = driver_->get_terminal_size();

    // and we need to know where the cursor is on startup
    // check cursor within dimensions?
    cursor_ = driver_->get_cursor_position();

    // install handler to update size when it changes
    install_resize_handler();
}

TerminalWindow::~TerminalWindow() { WINDOW.release(); }

void TerminalWindow::on_resize() {
    // make sure we defer the next SIGWINCH while handling the current one
    // because this function is not reentrant
    SignalGuard guard{signal_suspender_};

    auto dim_new = driver_->get_terminal_size();
    auto dim_old = dim_;
    dim_ = dim_new;

    if (resize_receiver_ != nullptr) {
        resize_receiver_->on_window_resize(dim_old, dim_new);
    }
}

const Dimensions &TerminalWindow::get_size() const { return dim_; }

const Point &TerminalWindow::get_cursor() const { return cursor_; }

void TerminalWindow::set_cursor(const Point &point) {
    check_is_on_window(point);

    driver_->set_cursor_position(point);
    cursor_ = point;
}

void TerminalWindow::put_char(const char &ch) {
    // check cursor pos leaves space for the char on the window?
    driver_->put_char(ch);
    // recalculate and update cursor_ ? (is_printable etc)
}

void TerminalWindow::put_uchar(const std::string &ch) {
    driver_->put_uchar(ch);
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

void TerminalWindow::register_resize_receiver(WindowResizeReceiver *receiver) {
    resize_receiver_ = receiver;
}

void TerminalWindow::check_is_on_window(const Point &point) {
    if ((point.x < 1) || (point.x > dim_.width) || (point.y < 1) ||
        (point.y > dim_.height)) {
        char *buf = PCHAR(calloc(512, 1));
        sprintf(
            buf,
            "TerminalWindow.check_is_on_window: tried to put cursor outside "
            "window, point: (%d, %d), x range: [%d, %d], y range: [%d, %d]",
            point.x, point.y, 1, dim_.width, 1, dim_.height);

        // Instead of throwing here we abort so that we can get a core dump and
        // investigate the dimensions of the window, the desired cursor position
        // etc.
        std::cerr << buf << '\n';
        std::abort();
        // throw std::out_of_range(buf);
    }
}

void TerminalWindow::install_resize_handler() {
    signal(SIGWINCH, TerminalWindow_signal_handler);
}

} // namespace termui
} // namespace bandwit