#pragma once
#include "types.hpp"

void initworkspace();

object* myalloc(object_type* type);
object* cons(object*, object*);

bool is_an_object(object* x);
bool consp(object* x);
bool listp(object* x);

void markobject(object*);
void gc(object*, object*);

template<typename T, typename U>
U iter_objects(T arg, U (*callback)(object* obj, T arg, bool* stop));

extern object* GCStack;
