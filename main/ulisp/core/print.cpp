#include "print.hpp"
#include "allocator.hpp"
#include "macros.hpp"
#include <stdio.h>

static void printlist(FILE* to, object* obj) {
    fprintf(to, "(");
    printobject(to, car(obj));
    obj = cdr(obj);
    for (; obj && listp(obj); obj = cdr(obj)) {
        fprintf(to, " ");
        printobject(to, car(obj));
    }
    if (obj) {
        fprintf(to, " . ");
        printobject(to, obj);
    }
    fprintf(to, ")");
}

void printobject(FILE* to, object* obj) {
    if (obj == nil) fprintf(to, "nil");
    else if (listp(obj)) printlist(to, obj);
    else {
        auto type = obj->typeinfo;
        if (!type || !callprintmethod(obj, to))  {
            fprintf(to, "<pointer:%p>", obj->pointer);
        }
    }
}
