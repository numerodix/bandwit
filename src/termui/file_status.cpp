#include <fcntl.h>
#include <stdexcept>

#include "file_status.hpp"

namespace bmon {
namespace termui {

void FileStatusSetter::set() {
    // get the current status
    int status = fcntl(fileno_, F_GETFL);
    if (status == -1) {
        throw std::runtime_error(
            "FileStatusSetter.set failed in fcntl() when trying to get flags");
    }

    // save the original status so we can restore it later
    orig_status_ = status;

    // the new status we want
    status = (status | status_on_) & ~status_off_;

    // It's already in the state we want - nothing to do here
    if (status == orig_status_) {
        return;
    }

    // Set the status we want
    int rv = fcntl(fileno_, F_SETFL, status);
    if (rv == -1) {
        throw std::runtime_error(
            "FileStatusSetter.set failed in fcntl() when trying to set flags");
    }
}

void FileStatusSetter::reset() {
    // get the current status
    int status = fcntl(fileno_, F_GETFL);
    if (status == -1) {
        throw std::runtime_error("FileStatusSetter.reset failed in fcntl() "
                                 "when trying to get flags");
    }

    // It's already in the state we want - nothing to do here
    if (status == orig_status_) {
        return;
    }

    // Restore the original status
    int rv = fcntl(fileno_, F_SETFL, orig_status_);
    if (rv == -1) {
        throw std::runtime_error("FileStatusSetter.reset failed in fcntl() "
                                 "when trying to set flags");
    }
}

FileStatusSet &FileStatusSet::status_on(int status_on) {
    status_on_ = status_on_ | status_on;
    return *this;
}

FileStatusSet &FileStatusSet::status_off(int status_off) {
    status_off_ = status_off_ | status_off;
    return *this;
}

FileStatusSetter FileStatusSet::build_setter(int fileno) {
    FileStatusSetter setter{fileno, status_on_, status_off_};
    return setter;
}

} // namespace termui
} // namespace bmon