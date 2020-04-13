#ifndef TERMINAL_SURFACE_H
#define TERMINAL_SURFACE_H

#include "termui/dimensions.hpp"
#include "termui/point.hpp"

namespace bmon {
namespace termui {

class TerminalWindow;

class TerminalSurface {
  public:
    TerminalSurface(TerminalWindow *win, uint16_t num_lines);

    void on_startup();
    void on_window_resize(const Dimensions &win_dim_old,
                          const Dimensions &win_dim_new);
    void on_carriage_return();

    void clear_surface();
    void put_char(const Point &point, const char &ch);
    void put_uchar(const Point& point, const std::string& ch);
    void flush();

    const Dimensions &get_size() const;
    const Point &get_upper_left() const;
    const Point &get_lower_left() const;

  private:
    void check_surface_fits(const Dimensions &win_dim);
    Dimensions recompute_dimensions(const Dimensions &win_dim) const;
    Point recompute_lower_left(const Point &upper_left) const;
    Point translate_point(const Point& point);

    TerminalWindow *win_{nullptr};

    // the minimum dimensions to display the surface
    uint16_t min_lines_{6};
    uint16_t min_cols_{30};

    uint16_t num_lines_{0};
    const char bg_char_ = ' ';

    Dimensions dim_{};
    Point upper_left_{};
    Point lower_left_{};
};

} // namespace termui
} // namespace bmon

#endif // TERMINAL_SURFACE_H
