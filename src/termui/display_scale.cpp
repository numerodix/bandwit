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

std::string get_label(DisplayScale scale) {
    switch (scale) {
    case DisplayScale::LINEAR:
        return "linear";
    case DisplayScale::LOG10:
        return "log10";
    case DisplayScale::LOG2:
        return "log2";
    }
}

} // namespace termui
} // namespace bandwit