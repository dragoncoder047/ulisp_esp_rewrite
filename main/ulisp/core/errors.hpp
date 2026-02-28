#pragma once
#include "types.hpp"
#include <setjmp.h>

extern jmp_buf toplevel_error_handler;
extern jmp_buf* current_error_handler;
extern table_entry_t* Context;

[[noreturn]] void errorsym(table_entry_t* fname, const char* const message, object* what);
[[noreturn]] void errorsym2(table_entry_t* fname, const char* const message);
#define error(str, sym) errorsym(Context, (str), (sym))
#define error2(str) errorsym2(Context, (str))
[[noreturn]] void formaterr(object* formatstr, const char* const message, int width);
