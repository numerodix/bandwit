#ifndef TERMINAL_SURFACE_H
#define TERMINAL_SURFACE_H

#include "termui/dimensions.h"
#include "termui/point.h"

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

} // namespace termui
} // namespace bmon

#endif // TERMINAL_SURFACE_H
