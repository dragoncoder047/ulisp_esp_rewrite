#include "types.hpp"
#include "panic.hpp"
#include "errors.hpp"
#include <stdio.h>

bool isinstance(object* obj, object_type* type) {
    if (!obj) return false;
    auto realtype = obj->typeinfo;
    while (realtype && realtype != type) realtype = realtype->super;
    return !!realtype;
}

bool callprintmethod(object* obj, FILE* to) {
    auto type = obj->typeinfo;
    while (type && !type->print) type = type->super;
    if (!type) return false;
    type->print(obj, to);
    return true;
}

object* callevalmethod(object* obj, object* env) {
    auto type = obj->typeinfo;
    while (type && !type->eval) type = type->super;
    if (!type) error("cannot evaluate", obj);
    return type->eval(obj, env);
}

object* callapplymethod(object* obj, object* args, object* env) {
    auto type = obj->typeinfo;
    while (type && !type->apply) type = type->super;
    if (!type) error("not callable", obj);
    return type->apply(obj, args, env);
}

mark_func_t getmarkmethod(object* obj) {
    auto type = obj->typeinfo;
    while (type && !type->mark) type = type->super;
    if (!type) return NULL;
    return type->mark;
}

object* cdr_is_never_pointer(object* obj) {
    (void)obj;
    return NULL;
}
