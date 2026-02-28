#include <stdio.h>
#include <stdlib.h>
#include "panic.hpp"

[[noreturn]] void ulisp_panic(const char* const message) {
    fprintf(stderr, "uLisp panic: %s", message);
    abort();
}
