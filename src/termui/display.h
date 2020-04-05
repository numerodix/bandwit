#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>

namespace bmon {
namespace termui {

class Display {
  public:
    explicit Display(uint16_t num_lines) : num_lines_{num_lines} {}

    void initialize();

  private:
    void detect_size();

    uint16_t num_lines_{0};
    uint16_t cols_{0};
    uint16_t rows_{0};
};

} // namespace termui
} // namespace bmon

#endif // DISPLAY_H