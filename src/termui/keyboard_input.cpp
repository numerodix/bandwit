#include <cstdio>
#include <cstring>
#include <thread>

#include "keyboard_input.hpp"

namespace bandwit {
namespace termui {

KeyPress KeyboardInputReader::read_keypress(Millis interval) {
    KeyPress key{KeyPress::NOTHING};

    // Sleep first to give the user time to press a key
    std::this_thread::sleep_for(interval);

    // Read the key press non blocking
    char chars[4096] = {0};
    int i = 0;

    char ch = fgetc(fl_);
    while (ch >= 0) {
        chars[i++] = ch;
        ch = fgetc(fl_);
    }

    if ((strlen(chars) == 1) && (chars[0] == '\n')) {
        key = KeyPress::CARRIAGE_RETURN;
    } else if ((strlen(chars) == 1) && (chars[0] == 'r')) {
        key = KeyPress::LETTER_R;
    } else if ((strlen(chars) == 1) && (chars[0] == 't')) {
        key = KeyPress::LETTER_T;
    } else if ((strlen(chars) == 1) && (chars[0] == 'c')) {
        key = KeyPress::LETTER_C;
    } else if ((strlen(chars) == 1) && (chars[0] == 's')) {
        key = KeyPress::LETTER_S;
    } else if ((strlen(chars) == 1) && (chars[0] == 'q')) {
        key = KeyPress::QUIT;
    } else if ((strlen(chars) == 3) && (chars[0] == '\033') &&
               chars[2] == 'A') {
        key = KeyPress::ARROW_UP;
    } else if ((strlen(chars) == 3) && (chars[0] == '\033') &&
               chars[2] == 'B') {
        key = KeyPress::ARROW_DOWN;
    } else if ((strlen(chars) == 3) && (chars[0] == '\033') &&
               chars[2] == 'C') {
        key = KeyPress::ARROW_RIGHT;
    } else if ((strlen(chars) == 3) && (chars[0] == '\033') &&
               chars[2] == 'D') {
        key = KeyPress::ARROW_LEFT;
    }
    return key;
}

KeyPress KeyboardInputReader::read_nonblocking(Millis interval) {
    // We have `interval` of time in which to read a key press. Once we read it
    // we immediately return it. If there is no input we keep spinning and
    // trying again.
    auto num_loops = interval / read_char_interval_;
    auto frac_read_interval = interval - (num_loops * read_char_interval_);

    // spin for num_loops + 1 and use the fractional interval in one of the
    // loops
    for (auto i = 0; i < num_loops + 1; i++) {
        auto sleep = i == 0 ? frac_read_interval : read_char_interval_;

        auto key = read_keypress(sleep);
        if (key != KeyPress::NOTHING) {
            return key;
        }
    }

    return KeyPress::NOTHING;
}

} // namespace termui
} // namespace bandwit
