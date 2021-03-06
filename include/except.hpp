#ifndef EXCEPT_H
#define EXCEPT_H

#include <cerrno>
#include <cstdio>
#include <cstring>

#include "macros.hpp"

#define MAX_ERROR_MESSAGE_LEN 512

#define THROW(ExcClass)                                                        \
    { throw ExcClass(); }

#define THROW_MSG(ExcClass, msg)                                               \
    {                                                                          \
        auto buf = PCHAR(malloc(MAX_ERROR_MESSAGE_LEN));                       \
        snprintf(buf, MAX_ERROR_MESSAGE_LEN, "%s:%d %s", __FILE__, __LINE__,   \
                 msg);                                                         \
        throw ExcClass(buf);                                                   \
    }

#define THROW_ARGS(ExcClass, msg, ...)                                         \
    {                                                                          \
        auto buf_msg = PCHAR(malloc(MAX_ERROR_MESSAGE_LEN));                   \
        snprintf(buf_msg, MAX_ERROR_MESSAGE_LEN, msg, __VA_ARGS__);            \
        auto buf = PCHAR(malloc(MAX_ERROR_MESSAGE_LEN));                       \
        snprintf(buf, MAX_ERROR_MESSAGE_LEN, "%s:%d %s", __FILE__, __LINE__,   \
                 buf_msg);                                                     \
        throw ExcClass(buf);                                                   \
    }

#define THROW_CERROR(ExcClass, msg)                                            \
    {                                                                          \
        int errno_orig = errno;                                                \
        auto err_msg = strerror(errno_orig);                                   \
        auto buf = PCHAR(malloc(MAX_ERROR_MESSAGE_LEN));                       \
        snprintf(buf, MAX_ERROR_MESSAGE_LEN, "%s:%d %s: [%d] %s", __FILE__,    \
                 __LINE__, msg, errno_orig, err_msg);                          \
        throw ExcClass(buf);                                                   \
    }

#endif // EXCEPT_H
