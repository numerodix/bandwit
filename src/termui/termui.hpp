#ifndef TERMUI_H
#define TERMUI_H

#include <chrono>
#include <string>

#include "sampling/sampler.hpp"
#include "sampling/time_series.hpp"
#include "termui/bar_chart.hpp"
#include "termui/file_status.hpp"
#include "termui/keyboard_input.hpp"
#include "termui/terminal_driver.hpp"
#include "termui/terminal_mode.hpp"
#include "termui/terminal_surface.hpp"
#include "termui/terminal_window.hpp"
#include "termui/window_resize.hpp"

namespace bmon {
namespace termui {

enum class DisplayMode {
    DISPLAY_RX,
    DISPLAY_TX,
};

class TermUi : public WindowResizeReceiver {
  public:
    explicit TermUi(const std::string& iface_name);
    ~TermUi();

    CLASS_DISABLE_COPIES(TermUi)
    CLASS_DISABLE_MOVES(TermUi)

    void run_forever();

    void sample();
    void render();
    void read_keyboard_input(Millis interval);

    void on_window_resize(const Dimensions &win_dim_old,
                          const Dimensions &win_dim_new) override;

  private:
    std::string iface_name_{};
    DisplayMode mode_{DisplayMode::DISPLAY_RX};
    std::chrono::seconds one_sec_{1};
    sampling::Sample prev_sample_{};

    std::unique_ptr<BarChart> bar_chart_{nullptr};
    std::unique_ptr<FileStatusSetter> blocking_status_setter_{nullptr};
    std::unique_ptr<FileStatusSetter> non_blocking_status_setter_{nullptr};
    std::unique_ptr<KeyboardInputReader> kb_reader_{nullptr};
    std::unique_ptr<SignalSuspender> susp_sigint_{nullptr};
    std::unique_ptr<SignalSuspender> susp_sigwinch_{nullptr};
    std::unique_ptr<TerminalDriver> terminal_driver_{nullptr};
    std::unique_ptr<TerminalModeSetter> mode_setter_{nullptr};
    std::unique_ptr<TerminalSurface> terminal_surface_{nullptr};
    std::unique_ptr<TerminalWindow> terminal_window_{nullptr};
    std::unique_ptr<sampling::Sampler> sampler_{nullptr};
    std::unique_ptr<sampling::TimeSeries> ts_rx_{nullptr};
    std::unique_ptr<sampling::TimeSeries> ts_tx_{nullptr};
};

} // namespace termui
} // namespace bmon

#endif // TERMUI_H
