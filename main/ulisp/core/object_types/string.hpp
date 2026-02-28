#pragma once
#include "../types.hpp"
#include <sys/types.h>

extern object_type string_type;
object* stringy_mark(object*);

object* newstring();
void buildstring(char ch, object** tail);
object* copystring(object* str);
object* lispstring(const char* const str);

object* checkstring(object* obj);

void plispstr(object* str, FILE* to);

ssize_t stringcompare(object* str1, object* str2, bool less, bool equal, bool greater);
bool stringequal(object* str1, object* str2);
