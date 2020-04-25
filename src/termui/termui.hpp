#ifndef TERMUI_H
#define TERMUI_H

#include <string>

#include "sampling/sampler.hpp"
#include "termui/bar_chart.hpp"
#include "termui/file_status.hpp"
#include "termui/terminal_driver.hpp"
#include "termui/terminal_mode.hpp"
#include "termui/terminal_surface.hpp"
#include "termui/terminal_window.hpp"

namespace bmon {
namespace termui {

enum class DisplayMode {
    DISPLAY_RX,
    DISPLAY_TX,
};

class TermUi {
  public:
    explicit TermUi(const std::string& iface_name);
    ~TermUi();

    void display_bar_chart();

  private:
    std::string iface_name_{};
    std::unique_ptr<sampling::Sampler> sampler_{nullptr};
    std::unique_ptr<BarChart> bar_chart_{nullptr};
    std::unique_ptr<FileStatusSetter> blocking_status_setter_{nullptr};
    std::unique_ptr<FileStatusSetter> non_blocking_status_setter_{nullptr};
    std::unique_ptr<SignalSuspender> susp_sigint_{nullptr};
    std::unique_ptr<SignalSuspender> susp_sigwinch_{nullptr};
    std::unique_ptr<TerminalDriver> terminal_driver_{nullptr};
    std::unique_ptr<TerminalModeSetter> mode_setter_{nullptr};
    std::unique_ptr<TerminalSurface> terminal_surface_{nullptr};
    std::unique_ptr<TerminalWindow> terminal_window_{nullptr};
};

} // namespace termui
} // namespace bmon

#endif // TERMUI_H
