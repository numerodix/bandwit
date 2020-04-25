#ifndef WINDOW_RESIZE_H
#define WINDOW_RESIZE_H

#include "dimensions.hpp"

namespace bandwit {
namespace termui {

class WindowResizeReceiver {
  public:
    WindowResizeReceiver() = default;
    virtual ~WindowResizeReceiver() = default;

    CLASS_DISABLE_COPIES(WindowResizeReceiver)
    CLASS_DISABLE_MOVES(WindowResizeReceiver)

    virtual void on_window_resize(const Dimensions &win_dim_old,
                                  const Dimensions &win_dim_new) = 0;
};

} // namespace termui
} // namespace bandwit

#endif // WINDOW_RESIZE_H
