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