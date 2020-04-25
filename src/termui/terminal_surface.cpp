#include <stdexcept>

#include "macros.hpp"
#include "terminal_surface.hpp"
#include "terminal_window.hpp"

namespace bmon {
namespace termui {

TerminalSurface::TerminalSurface(TerminalWindow *win, uint16_t num_lines)
    : win_{win}, num_lines_{num_lines} {
    // win_->register_surface(this);
    win_->register_resize_receiver(this);
    on_startup();
}

void TerminalSurface::on_startup() {
    auto win_dim = win_->get_size();
    auto win_cur = win_->get_cursor();

    // Fail fast if the window size cannot fit the surface
    check_surface_fits(win_dim);

    // Detect if the bottom of the surface would currently overshoot the
    // terminal height - if so we need to force scroll the terminal by the
    // amount of the overshoot.
    int overshoot = INT(win_cur.y) + INT(num_lines_) - INT(win_dim.height);

    if (overshoot > 0) {
        for (int y = 0; y < overshoot; ++y) {
            for (auto x = 1; x <= win_dim.width; ++x) {
                win_->put_char(' '); /// XXX
            }
        }
    }
    win_->flush();

    // The upper left is normally just where the cursor was
    auto upper_left_y = win_cur.y;

    // ...but if we had to force scroll then we have to calculate it based on
    // the overshoot because the scroll made the cursor effectively shift
    // upwards.
    if (overshoot > 0) {
        upper_left_y = U16(INT(win_cur.y) - INT(overshoot) + 1);
    }

    // Initialize all positional invariants
    dim_ = recompute_dimensions(win_dim);
    upper_left_ = Point{win_cur.x, upper_left_y};
    lower_left_ = recompute_lower_left(upper_left_);

    clear_surface();
}

void TerminalSurface::on_window_resize(const Dimensions &win_dim_old,
                                       const Dimensions &win_dim_new) {
    // Fail fast if the new size cannot fit the surface
    check_surface_fits(win_dim_new);

    // The terminal height has changed
    int delta = INT(win_dim_new.height) - INT(win_dim_old.height);

    if (delta > 0) {
        // The terminal has become taller - we extend the surface to fill the
        // empty space below!
        num_lines_ = num_lines_ + U16(delta);

    } else if (delta < 0) {
        // The terminal has become shorter - terminals expect the cursor to
        // shift upwards by clearing the space above the surface. We let the
        // surface stay the same size, but move it up accordingly.
        int upper_left_y = INT(upper_left_.y) - abs(delta);

        // Is the terminal now too short to fit our current num_lines_? If so we
        // need to reduce the surface to fill the size of the terminal.
        if (upper_left_y < 1) {
            upper_left_y = 1;
            num_lines_ = win_dim_new.height;
        }

        upper_left_.y = U16(upper_left_y);
    }

    lower_left_ = recompute_lower_left(upper_left_);
    dim_ = recompute_dimensions(win_dim_new);

    clear_surface();
}

void TerminalSurface::on_carriage_return() {
    // When a carriage return occurs we take it as a cue to force scroll the
    // terminal and increase the surface by one line
    auto win_dim = win_->get_size();

    // Is the surface already occupying the whole terminal window? If so we do
    // nothing.
    if (num_lines_ >= win_dim.height) {
        return;
    }

    // Force scroll by one line
    auto lower_left = get_lower_left();
    win_->set_cursor(lower_left);

    // Fill the current line and the next
    for (auto y = 0; y < 2; ++y) {
        for (auto x = 1; x <= win_dim.width; ++x) {
            win_->put_char(' ');
        }
    }

    win_->flush();

    // Update positional invariants
    num_lines_ += 1;
    dim_ = recompute_dimensions(win_dim);
    upper_left_.y -= 1;
    lower_left_ = recompute_lower_left(upper_left_);
}

void TerminalSurface::clear_surface() {
    auto dim = get_size();
    auto upper_left = get_upper_left();
    auto lower_left = get_lower_left();

    win_->set_cursor(upper_left);

    for (int y = 0; y < num_lines_; ++y) {
        for (int x = 1; x <= dim.width; ++x) {
            win_->put_char(bg_char_);
        }
    }

    win_->set_cursor(lower_left);
    win_->flush();
}

void TerminalSurface::put_char(const Point &point, const char &ch) {
    auto point_win = translate_point(point);

    win_->set_cursor(point_win);
    win_->put_char(ch);
}

void TerminalSurface::put_uchar(const Point &point, const std::string &ch) {
    auto point_win = translate_point(point);

    win_->set_cursor(point_win);
    win_->put_uchar(ch);
}

void TerminalSurface::flush() {
    auto lower_left = get_lower_left();
    win_->set_cursor(lower_left);

    win_->flush();
}

const Dimensions &TerminalSurface::get_size() const { return dim_; }

const Point &TerminalSurface::get_upper_left() const { return upper_left_; }

const Point &TerminalSurface::get_lower_left() const { return lower_left_; }

void TerminalSurface::check_surface_fits(const Dimensions &win_dim) {
    if ((win_dim.width < min_cols_) || (win_dim.height < min_lines_)) {
        throw std::runtime_error(
            "TerminalSurface.check_surface_fits: terminal window too small :(");
    }
}

Dimensions
TerminalSurface::recompute_dimensions(const Dimensions &win_dim) const {
    Dimensions dim{win_dim.width, num_lines_};
    return dim;
}

Point TerminalSurface::recompute_lower_left(const Point &upper_left) const {
    Point lower_left{
        U16(1),
        U16(INT(upper_left.y) + INT(num_lines_) - 1),
    };
    return lower_left;
}

Point TerminalSurface::translate_point(const Point &point) {
    auto upper_left = get_upper_left();

    // Map surface coordinates onto window coordinates
    Point point_win{
        point.x,
        U16(INT(upper_left.y) + INT(point.y) - 1),
    };

    return point_win;
}

} // namespace termui
} // namespace bmon