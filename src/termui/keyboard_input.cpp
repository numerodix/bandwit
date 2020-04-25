#include <cstdio>
#include <thread>

#include "keyboard_input.hpp"

namespace bandwit {
namespace termui {

KeyPress KeyboardInputReader::read_char(Millis interval) {
    KeyPress key{KeyPress::NOTHING};

    // Sleep first to give the user time to press a key
    std::this_thread::sleep_for(interval);

    // Read the key press non blocking
    char ch = fgetc(fl_);

    switch (ch) {
    case '\n':
        key = KeyPress::CARRIAGE_RETURN;
        break;
    case 'r':
        key = KeyPress::DISPLAY_RX;
        break;
    case 't':
        key = KeyPress::DISPLAY_TX;
        break;
    case 'q':
        key = KeyPress::QUIT;
        break;
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

        auto key = read_char(sleep);
        if (key != KeyPress::NOTHING) {
            return key;
        }
    }

    return KeyPress::NOTHING;
}

} // namespace termui
} // namespace bandwit
