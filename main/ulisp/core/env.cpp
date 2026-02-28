#include "env.hpp"
#include "macros.hpp"
#include "allocator.hpp"
#include "object_types/symbol.hpp"

object* tee;
object* GlobalEnv;

object_type tee_type = {
    .eval = [](object* obj, object* env) -> object* {
        (void)obj, (void)env;
        return tee;
    },
    .print = [](object* obj, FILE* to) -> void {
        (void)obj;
        fputc('t', to);
    },
};

object* value(object* name, object* env) {
    for (; env; env = cdr(env)) {
        auto pair = car(env);
        if (pair && eqsymbol(car(pair), name)) return pair;
    }
    return nil;
}

void initenv() {
    tee = myalloc(&tee_type);
}
