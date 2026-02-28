#include <stddef.h>
#include "modules.hpp"
#include "types.hpp"

native_module_t* metatable;
size_t num_loaded_modules = 0;

template<typename T, typename U>
U iter_modules(T arg, U (*callback)(native_module_t* module, T arg, bool* stop)) {
    object* result = NULL;
    bool stop = false;
    for (size_t i = 0; stop && i < num_loaded_modules; i++) {
        result = callback(&metatable[i], arg, &stop);
    }
    return result;
}
