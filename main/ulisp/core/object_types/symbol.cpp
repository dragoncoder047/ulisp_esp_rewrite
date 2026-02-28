#include "symbol.hpp"
#include "../types.hpp"
#include "../allocator.hpp"
#include "../modules.hpp"
#include "../panic.hpp"
#include "string.hpp"
#include <stdint.h>
#include <string.h>
#include <ctype.h>
// why the heck do C++ internals use 'car' as a field name
#undef car
#include <math.h>
#include "../macros.hpp"


// The parent symbol type for all symbols.
object_type symbol_type = {
    .abstract_type = true,
};

// Built-in symbol - cdr is pointer to table entry.
object_type builtin_symbol_type = {
    .super = &symbol_type,
    .print = [](object* obj, FILE* to) -> void {
        fprintf(to, "%s", obj->builtin->name);
    },
};

// maximum number such that 40^n < 2^32
#define MAX_CHARS_PER_PACK40 6
static char fromradix40(char n);

// Packed symbol - cdr is base40-encoded name.
object_type packed_symbol_type = {
    .super = &symbol_type,
    .mark = cdr_is_never_pointer,
    .print = [](object* sym, FILE* to) -> void {
        uint32_t x = sym->name;
        for (int d = pow(40, MAX_CHARS_PER_PACK40 - 1); d > 0; d /= 40) {
            uint32_t j = x / d;
            char c = fromradix40(j);
            if (c == 0) return;
            fputc(c, to);
            x = x - j*d;
        }
    },
};

// Long symbol - cdr is same as a string cdr.
object_type long_symbol_type = {
    .super = &symbol_type,
    .mark = stringy_mark,
    .print = plispstr,
};

bool eqsymbols(object* symbol, const char* const name) {
    if (symbol->typeinfo == &long_symbol_type) {
        size_t i = 0;
        size_t j = 0;
        auto string = cdr(symbol);
        for (; string != nil && name[i] > 0; i++) {
            if (name[i] != string->chars.chars[j]) return false;
            j++;
            if (j >= string->chars.used) {
                j = 0;
                string = car(string);
            }
        }
        return true;
    }
    if (symbol->typeinfo == &packed_symbol_type) {
        if (strlen(name) > MAX_CHARS_PER_PACK40) return false;
        uint32_t x = symbol->name;
        // Thankfully, pow() has a constexpr form so this is optimized to a single constant
        for (int d = pow(40, MAX_CHARS_PER_PACK40 - 1), i = 0; d > 0; d /= 40) {
            uint32_t j = x / d;
            if (name[i] != fromradix40(j)) return false;
            if (name[i]) i++;
            x -= j * d;
        }
        return true;
    }
    if (symbol->typeinfo == &builtin_symbol_type) {
        return !strcmp(symbol->builtin->name, name);
    }
    ulisp_panic("eqsymbols called on non-symbol");
}

bool eqsymbol(object* sym1, object* sym2) {
    if (sym1->typeinfo != sym2->typeinfo) return false;
    if (sym1->typeinfo == &builtin_symbol_type || sym1->typeinfo == &packed_symbol_type) {
        return sym1->name == sym2->name;
    }
    return stringequal(sym1, sym2);
}

static object* builtin(table_entry_t* entry) {
    auto obj = myalloc(&builtin_symbol_type);
    obj->builtin = entry;
    return obj;
}

static table_entry_t* lookupbuiltin(const char* const string) {
    return iter_modules<const char* const, table_entry_t*>(string, [](native_module_t* module, const char* const name, bool* stop) -> table_entry_t* {
        auto table = module->functions;
        if (table) {
            for (size_t i = 0; table[i] != NULL; i++) {
                if (!strcmp(table[i]->name, name)) {
                    *stop = true;
                    return table[i];
                }
            }
        }
        return NULL;
    });
}

static int8_t toradix40(char ch) {
    if (ch == 0) return 0;
    if (ch >= '0' && ch <= '9') return ch - '0' + 1;
    if (ch == '-') return 37;
    if (ch == '*') return 38;
    if (ch == '$') return 39;
    ch = ch | 0x20; // Force lowercase
    if (ch >= 'a' && ch <= 'z') return ch - 'a' + 11;
    return -1; // Invalid
}

static char fromradix40(char n) {
    if (n >= 1 && n <= 10) return '0' + n - 1;
    if (n >= 11 && n <= 36) return 'a' + n - 11;
    if (n == 37) return '-';
    if (n == 38) return '*';
    if (n == 39) return '$';
    return 0;
}

static packed_symbol_t pack40(const char* const buffer) {
    packed_symbol_t x = 0;
    int j = 0;
    for (int i = 0; i < 6; i++) {
        x *= 40;
        x += toradix40(buffer[j]);
        if (buffer[j] != 0) j++;
    }
    return x;
}

static bool valid40(const char* const buffer) {
    if (strlen(buffer) > 6) return false;
    int t = 11;
    for (int i = 0; i < 6; i++) {
        if (toradix40(buffer[i]) < t) return false;
        if (buffer[i] == 0) break;
        t = 0;
    }
    return true;
}

static object* packedsym(packed_symbol_t name) {
    auto obj = myalloc(&packed_symbol_type);
    obj->name = name;
    return obj;
}

object* intern(const char* const name) {
    object* obj = iter_objects<const char* const, object*>(name, [](object* obj, const char* const name, bool* stop) -> object* {
        if (symbolp(obj) && eqsymbols(obj, name)) {
            *stop = true;
            return obj;
        }
        return NULL;
    });
    if (!obj) obj = encode_symbol(name);
    return obj;
}

object* encode_symbol(const char* const name) {
    table_entry_t* row = lookupbuiltin(name);
    if (row != NULL) return builtin(row);
    if (valid40(name)) return packedsym(pack40(name));
    auto obj = lispstring(name);
    obj->typeinfo = &long_symbol_type;
    return obj;
}
