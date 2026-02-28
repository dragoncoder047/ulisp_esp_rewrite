#pragma once
#include <stdint.h>
#include <stdio.h>
#include "../ulisp_options.hpp"



typedef struct object object;
typedef struct object_type object_type;
typedef struct table_entry_t table_entry_t;

typedef object* (*function_ptr_t)(object* args, object* env);

typedef object* (*mark_func_t)(object*);
typedef object* (*eval_func_t)(object* obj, object* env);
typedef object* (*apply_func_t)(object* obj, object* args, object* env);
typedef void (*print_func_t)(object* obj, FILE* to);

struct object_type {
    object_type* super;
    bool abstract_type : 1;
    bool special_form : 1;
    mark_func_t mark;
    eval_func_t eval;
    apply_func_t apply;
    print_func_t print;
};

typedef uint32_t packed_symbol_t;

typedef struct {
    uint8_t used;
    char chars[(sizeof(void*) - 1)];
} string_chunk_t;

struct object {
    union {
        object* car;
        object_type* typeinfo;
    };
    union {
        object* cdr;
        packed_symbol_t name;
        int integer;
        float single_float;
        string_chunk_t chars;
        FILE* stream;
        table_entry_t* builtin;
        void* pointer;
    };
};

typedef enum : uint8_t {
    KEYWORD_TYPE,
    FUNCTION_TYPE,
    MACRO_TYPE,
    SPECIAL_SYMBOL_TYPE,
} form_kind_t;

struct table_entry_t {
    const char* const name;
    function_ptr_t impl;
    form_kind_t kind;
    const char* const docstring;
};

bool isinstance(object*, object_type*);
bool callprintmethod(object*, FILE*);
object* callevalmethod(object* obj, object* env);
object* callapplymethod(object* obj, object* args, object* env);
mark_func_t getmarkmethod(object* obj);

object* cdr_is_never_pointer(object* obj);


// sanity check
static_assert(sizeof(object) == 2 * sizeof(void*));
static_assert(sizeof(string_chunk_t) == sizeof(void*));
