#pragma once
#include <stddef.h>
#include "types.hpp"
#include "streams.hpp"

typedef struct {
    const char* const name;
    table_entry_t** const functions;
    stream_kind_t** const streams;
    void (*setup)();
    void (*pre_garbage_collect)();
} native_module_t;

extern native_module_t* metatable;

template<typename T, typename U>
U iter_modules(T arg, U (*callback)(native_module_t* module, T arg, bool* stop));

