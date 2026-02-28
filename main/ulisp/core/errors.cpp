#include "errors.hpp"
#include "allocator.hpp"
#include "print.hpp"
#include "flags.hpp"
#include <setjmp.h>
#include <stdio.h>

jmp_buf toplevel_error_handler;
jmp_buf* current_error_handler = &toplevel_error_handler;
table_entry_t* Context;

static void errorsub(table_entry_t* fname, const char* const message) {
    if (fname != NULL) fprintf(stderr, "Error in %s: %s", fname->name, message);
    else fprintf(stderr, "Error: %s", message);
}

[[noreturn]] static void errorend() {
    GCStack = NULL;
    longjmp(*current_error_handler, 1);
}

[[noreturn]] void errorsym(table_entry_t* fname, const char* const message, object* what) {
    if (!Flags.inside_error_handler) {
        errorsub(fname, message);
        fprintf(stderr, ": ");
        printobject(stderr, what);
        fprintf(stderr, "\n");
    }
    errorend();
}

[[noreturn]] void errorsym2(table_entry_t* fname, const char* const message) {
    if (!Flags.inside_error_handler) {
        errorsub(fname, message);
        fprintf(stderr, "\n");
    }
    errorend();
}

[[noreturn]] void formaterr(object* formatstr, const char* const message, int width) {
    fprintf(stderr, "    ");
    printobject(stderr, formatstr);
    fprintf(stderr, "%*s\n", width + 6, "^");
    error2(message);
}
