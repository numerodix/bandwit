#ifndef TERMUI_H
#define TERMUI_H

#include <chrono>
#include <string>

#include "sampling/agg_window.hpp"
#include "sampling/sampler.hpp"
#include "sampling/statistic.hpp"
#include "sampling/time_series_coll.hpp"
#include "termui/bar_chart.hpp"
#include "termui/display_mode.hpp"
#include "termui/display_scale.hpp"
#include "termui/file_status.hpp"
#include "termui/keyboard_input.hpp"
#include "termui/terminal_driver.hpp"
#include "termui/terminal_mode.hpp"
#include "termui/terminal_surface.hpp"
#include "termui/window_resize.hpp"

namespace bandwit {
namespace termui {

class TermUi : public WindowResizeReceiver {
    using AggregationWindow = sampling::AggregationWindow;
    using Statistic = sampling::Statistic;
    using TimeSeriesCollection = sampling::TimeSeriesCollection;
    using TimeSeriesSlice = sampling::TimeSeriesSlice;

  public:
    explicit TermUi(const std::string &iface_name);
    ~TermUi() override;

    CLASS_DISABLE_COPIES(TermUi)
    CLASS_DISABLE_MOVES(TermUi)

    void on_window_resize(const Dimensions &win_dim_old,
                          const Dimensions &win_dim_new) override;

    void run_forever();

  private:
    void sample();
    void render();
    void read_keyboard_input(Millis interval);

    void render_no_winch();
    void scroll_left();
    void scroll_right();
    void check_cursor();
    int historical_points() const;

    std::string iface_name_{};

    // Cursor at 0 means we are in dynamic update mode.
    // Cursor at >0 means that we are scrolling to the left through historical
    // data.
    int scroll_cursor_{0};

    DisplayMode display_mode_{DisplayMode::DISPLAY_RX};
    DisplayScale display_scale_{DisplayScale::LINEAR};
    Statistic stat_mode_{Statistic::AVERAGE};
    AggregationWindow agg_window_{AggregationWindow::ONE_SECOND};

    sampling::Sample prev_sample_{};

    std::unique_ptr<BarChart> bar_chart_{nullptr};
    std::unique_ptr<FileStatusSetter> blocking_status_setter_{nullptr};
    std::unique_ptr<FileStatusSetter> non_blocking_status_setter_{nullptr};
    std::unique_ptr<KeyboardInputReader> kb_reader_{nullptr};
    std::unique_ptr<SignalSuspender> susp_sigint_{nullptr};
    std::unique_ptr<SignalSuspender> susp_sigwinch_{nullptr};
    std::unique_ptr<TerminalDriver> terminal_driver_{nullptr};
    std::unique_ptr<TerminalModeSetter> interactive_mode_setter_{nullptr};
    std::unique_ptr<TerminalSurface> terminal_surface_{nullptr};
    std::unique_ptr<sampling::Sampler> sampler_{nullptr};

    std::unique_ptr<TimeSeriesCollection> ts_coll_rx_{nullptr};
    std::unique_ptr<TimeSeriesCollection> ts_coll_tx_{nullptr};
};

} // namespace termui
} // namespace bandwit

#endif // TERMUI_H
