#ifndef MACROS_H
#define MACROS_H

#include <chrono>
#include <unistd.h>

#define CLASS_DISABLE_COPY_CONSTRUCTOR(name) name(const name &) = delete;
#define CLASS_DISABLE_COPY_ASSIGNMENT(name)                                    \
    name &operator=(const name &) = delete;

#define CLASS_DISABLE_MOVE_CONSTRUCTOR(name) name(name &&) = delete;
#define CLASS_DISABLE_MOVE_ASSIGNMENT(name) name &operator=(name &&) = delete;

#define CLASS_DISABLE_COPIES(name)                                             \
    CLASS_DISABLE_COPY_CONSTRUCTOR(name)                                       \
    CLASS_DISABLE_COPY_ASSIGNMENT(name)

#define CLASS_DISABLE_MOVES(name)                                              \
    CLASS_DISABLE_MOVE_CONSTRUCTOR(name)                                       \
    CLASS_DISABLE_MOVE_ASSIGNMENT(name)

#define F64(num) static_cast<double>(num)

#define INT(num) static_cast<int>(num)
#define U16(num) static_cast<uint16_t>(num)
#define U32(num) static_cast<uint32_t>(num)
#define U64(num) static_cast<uint64_t>(num)

#define SIZE_T(num) static_cast<std::size_t>(num)

#define PCHAR(val) static_cast<char *>(val)

#define MILLIS(val) std::chrono::duration_cast<std::chrono::milliseconds>(val)

#endif // MACROS_H
