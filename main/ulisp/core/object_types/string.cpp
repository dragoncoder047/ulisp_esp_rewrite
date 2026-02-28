#include "string.hpp"
#include "../types.hpp"
#include "../macros.hpp"
#include "../allocator.hpp"
#include "../flags.hpp"
#include "../errors.hpp"
#include <stdio.h>
#include <string.h>

object* stringy_mark(object* str) {
    str = cdr(str);
    while (str) {
        auto temp = car(str);
        mark(str);
        str = temp;
    }
    return nil;
};

object_type string_type = {
    .mark = stringy_mark,
    .print = [](object* str, FILE* to) -> void {
        if (Flags.printing_readably) fputc('"', to);
        plispstr(str, to);
        if (Flags.printing_readably) fputc('"', to);
    },
};

object* newstring() {
    return myalloc(&string_type);
}

void buildstring(char ch, object** tail) {
    object* cell;
    if (cdr(*tail) == nil) {
        cell = cdr(*tail) = myalloc(NULL);
    } else if ((*tail)->chars.used < (sizeof(void*) - 1)) {
        (*tail)->chars.chars[(*tail)->chars.used] = ch;
        (*tail)->chars.used++;
        return;
    } else {
        cell = car(*tail) = myalloc(NULL);
    }
    car(cell) = nil;
    cell->chars.chars[0] = ch;
    cell->chars.used = 1;
    *tail = cell;
}

object* copystring(object* str) {
    object* obj = newstring();
    object* ptr = obj;
    str = cdr(str);
    while (str) {
        object* cell = myalloc(NULL);
        car(cell) = nil;
        if (cdr(obj) == nil) cdr(obj) = cell;
        else car(ptr) = cell;
        ptr = cell;
        ptr->chars = str->chars;
        str = car(str);
    }
    return obj;
}

object* lispstring(const char* const str) {
    object* obj = newstring();
    object* tail = obj;
    size_t i = 0;
    for(;;) {
        char ch = str[i];
        if (!ch) break;
        if (ch == '\\') ch = str[++i];
        buildstring(ch, &tail);
    }
    return obj;
}

object* checkstring(object* str) {
    if (!isinstance(str, &string_type)) error("expected a string", str);
    return str;
}

void plispstr(object* str, FILE* to) {
    object* form = cdr(str);
    while (form != NULL) {
        auto chars = form->chars;
        for (uint8_t i = 0; i < chars.used; i++) {
            if (Flags.printing_readably && (chars.chars[i] == '"' || chars.chars[i] == '\\')) {
                fputc('\\', to);
            }
            fputc(chars.chars[i], to);
        }
    }
}

bool stringequal(object* str1, object* str2) {
    str1 = cdr(str1);
    str2 = cdr(str2);
    while (str1 || str2) {
        if (!str1 || !str2) return false;
        if (strncmp(str1->chars.chars, str2->chars.chars, (sizeof(void*) - 1))) return false;
        str1 = car(str1);
        str2 = car(str2);
    }
    return true;
}
