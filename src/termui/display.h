#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>
#include <memory>

#include <termios.h>

#include "macros.h"
#include "surface.h"
#include "termui/point.h"

namespace bmon {
namespace termui {

class Display {
  public:
    explicit Display(uint16_t num_lines) : num_lines_{num_lines} {}
    ~Display();

    CLASS_DISABLE_COPIES(Display)
    CLASS_DISABLE_MOVES(Display)

    void initialize();

  private:
    // terminal mode routines
    void enter_cbreak_mode();
    void reset_term_mode();

    // display routines
    void recreate_surface();
    void detect_terminal_size();
    Point detect_terminal_cursor();
    uint16_t allocate_lines_for_surface(Point origin);

    uint16_t num_lines_{0};
    uint16_t cols_{0};
    uint16_t rows_{0};

    std::unique_ptr<Surface> surface_{nullptr};

    enum class TerminalMode { ORIGINAL, CBREAK };
    TerminalMode terminal_mode_{TerminalMode::ORIGINAL};
    struct termios stdin_orig_termios_ {};
};

} // namespace termui
} // namespace bmon

#endif // DISPLAY_H