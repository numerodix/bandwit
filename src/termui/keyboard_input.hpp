#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "aliases.hpp"

namespace bandwit {
namespace termui {

enum class KeyPress {
    NOTHING,
    CARRIAGE_RETURN,
    DISPLAY_RX,
    DISPLAY_TX,
    CYCLE_AGG_INTERVAL,
    QUIT,
};

class KeyboardInputReader {
  public:
    explicit KeyboardInputReader(FILE *fl) : fl_{fl} {}

    KeyPress read_nonblocking(Millis interval);

  private:
    KeyPress read_char(Millis interval);

    // Where to read the char from
    FILE *fl_;

    // How much time to allow for a single key press
    Millis read_char_interval_{10};
};

} // namespace termui
} // namespace bandwit

#endif // KEYBOARD_INPUT_H
