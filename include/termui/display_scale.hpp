#ifndef DISPLAY_SCALE_H
#define DISPLAY_SCALE_H

namespace bandwit {
namespace termui {

enum class DisplayScale {
    LINEAR,
    LOG10,
    LOG2,
};

DisplayScale next_scale(DisplayScale scale);

} // namespace termui
} // namespace bandwit

#endif // DISPLAY_SCALE_H
