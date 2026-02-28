#include "builtin_form.hpp"

object_type builtin_type = {
    .abstract_type = true,
    .print = [](object* obj, FILE* to) -> void {
        fprintf(to, "<built-in%s %s>",
                obj->typeinfo == &builtin_function_type ? " function"
                : obj->typeinfo == &builtin_keyword_type ? " keyword"
                : obj->typeinfo == &builtin_macro_type ? " macro"
                : obj->typeinfo == &builtin_special_symbol_type ? " special symbol"
                : "", obj->builtin->name);
    },
};

static object* call_builtin(object* obj, object* args, object* env) {
    return obj->builtin->impl(args, env);
}

object_type builtin_function_type = {
    .super = &builtin_type,
    .apply = call_builtin,
};

object_type builtin_macro_type = {
    .super = &builtin_type,
    .special_form = true,
    .apply = call_builtin,
};

object_type builtin_special_symbol_type = {
    .super = &builtin_type,
    .eval = [](object* obj, object* env) -> object* {
        return obj->builtin->impl(NULL, env);
    },
};

object_type builtin_keyword_type = {
    .super = &builtin_type,
};
