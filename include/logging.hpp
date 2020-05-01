#ifndef LOGGING_H
#define LOGGING_H

#include <cstdlib>
#include <fstream>

#include "macros.hpp"

#define LOGFILE "log"
#define MAX_LINE_LEN 1024

#define LOG(msg)                                                               \
    {                                                                          \
        std::fstream logfile{LOGFILE, std::fstream::out | std::fstream::app};  \
        logfile << msg;                                                        \
    }

#define LOG_A(msg, ...)                                                        \
    {                                                                          \
        std::fstream logfile{LOGFILE, std::fstream::out | std::fstream::app};  \
        auto buf_msg = PCHAR(malloc(MAX_LINE_LEN));                            \
        snprintf(buf_msg, MAX_LINE_LEN, msg, __VA_ARGS__);                     \
        logfile << buf_msg;                                                    \
    }

#endif // LOGGING_H
