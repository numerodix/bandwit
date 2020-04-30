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
    LETTER_R,
    LETTER_T,
    LETTER_C,
    LETTER_S,
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    QUIT,
};

class KeyboardInputReader {
  public:
    explicit KeyboardInputReader(FILE *fl) : fl_{fl} {}

    KeyPress read_nonblocking(Millis interval);

  private:
    KeyPress read_keypress(Millis interval);

    // Where to read the char from
    FILE *fl_;

    // How much time to allow for a single key press
    Millis read_char_interval_{10};
};

} // namespace termui
} // namespace bandwit

#endif // KEYBOARD_INPUT_H
