#ifndef EXCEPT_H
#define EXCEPT_H

#include <cstdio>
#include <errno.h>
#include <string.h>

#include "macros.hpp"

#define MAX_ERROR_MESSAGE_LEN 512

#define THROW(ExcClass)                                                        \
    { throw ExcClass(); }

#define THROW_MSG(ExcClass, msg)                                               \
    { throw ExcClass(msg); }

// FIXME: handle rv from malloc and sprintf (or snprintf?)
#define THROW_ARGS(ExcClass, msg, ...)                                         \
    {                                                                          \
        auto buf = PCHAR(malloc(MAX_ERROR_MESSAGE_LEN));                       \
        snprintf(buf, MAX_ERROR_MESSAGE_LEN, msg, __VA_ARGS__);                \
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
