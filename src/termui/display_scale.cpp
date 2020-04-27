#include "termui/display_scale.hpp"

namespace bandwit {
namespace termui {

DisplayScale next_scale(DisplayScale scale) {
    switch (scale) {
    case DisplayScale::LINEAR:
        return DisplayScale::LOG10;
    case DisplayScale::LOG10:
        return DisplayScale::LOG2;
    case DisplayScale::LOG2:
        return DisplayScale::LINEAR;
    }
}

} // namespace termui
} // namespace bandwit