#pragma once
#include "../types.hpp"
#include <stdio.h>

extern object_type symbol_type;

bool eqsymbols(object* symbol, const char* const name);
bool eqsymbol(object* sym1, object* sym2);

object* encode_symbol(const char* const name);
