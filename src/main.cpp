#include <csignal>
#include <iostream>
#include <unistd.h>

#include "termui/signals.hpp"
#include "termui/termui.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Must pass <iface_name>\n";
        exit(EXIT_FAILURE);
    }

    std::string iface_name{argv[1]};

    // We expect to get a Ctrl+C. Install a SIGINT handler that throws an
    // exception such that we can unwind orderly and enter the catch block
    // below.
    signal(SIGINT, bmon::termui::sigint_handler);

    // We desperately need to wrap the execution in a try/catch otherwise an
    // uncaught exception will terminate the program bypassing all destructors
    // and leave the terminal in a corrupted state.
    try {
        bmon::termui::TermUi termui{iface_name};
        termui.display_bar_chart();
    } catch (bmon::termui::InterruptException &e) {
        // This is the expected way to stop the program.
        // Emit a newline so we move beyond the menu that was displayed at the
        // cursor position.
        std::cerr << "\n";
    } catch (std::exception &e) {
        std::cerr << "\nTrapped uncaught exception:\n  " << e.what() << "\n";
        exit(EXIT_FAILURE);
    } catch (...) {
        std::cerr << "\nThis is the last resort exception handler. I have no "
                     "state about the error.";
        exit(EXIT_FAILURE);
    }

    return 0;
}
