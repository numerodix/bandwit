#ifndef FILE_STATUS_H
#define FILE_STATUS_H

#include <memory>

#include <macros.hpp>

namespace bandwit {
namespace termui {

class FileStatusSetter {
  public:
    explicit FileStatusSetter(int fileno, int status_on, int status_off) :
        fileno_{fileno}, status_on_{status_on}, status_off_{status_off} {}

    void set();
    void reset();

  private:
    int fileno_{0};
    int status_on_{0};
    int status_off_{0};
    int orig_status_{0};
};

class FileStatusSet {
  public:
    FileStatusSet& status_on(int status_on);
    FileStatusSet& status_off(int status_off);
    std::unique_ptr<FileStatusSetter> build_setter(int fileno);

  private:
    int status_on_{0};
    int status_off_{0};
};

class FileStatusGuard {
  public:
    explicit FileStatusGuard(FileStatusSetter* setter) : setter_{setter} {
        setter_->set();
    }
    ~FileStatusGuard() {
        setter_->reset();
    }

    CLASS_DISABLE_COPIES(FileStatusGuard)
    CLASS_DISABLE_MOVES(FileStatusGuard)

  private:
    FileStatusSetter* setter_{nullptr};
};

} // namespace termui
} // namespace bandwit

#endif // FILE_STATUS_H
