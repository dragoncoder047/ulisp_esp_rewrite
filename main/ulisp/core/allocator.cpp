#include <malloc.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include "allocator.hpp"
#include "macros.hpp"
#include "typetypes.hpp"
#include "panic.hpp"
#include "modules.hpp"
#include "errors.hpp"
#include "env.hpp"

typedef struct chunk_t chunk_t;
struct chunk_t {
    chunk_t* next;
    bool in_use;
    object objects[CHUNK_SIZE];
};

chunk_t* Workspace __attribute__((aligned(sizeof(void*))));
// For checking whether a pointer is to an object or not
object* first_object;
object* last_object;

size_t Freespace = 0;
size_t FreelistLength = 0;

object* Freelist;
chunk_t* FreeChunks;
object* GCStack;

#define _OBJ_IN_FREELIST_TYPE ((object*)((1ULL << (sizeof(void*) << 3)) - 1))

void initworkspace() {
    Workspace = (chunk_t*)calloc(NUM_CHUNKS, sizeof(chunk_t));
    if (!Workspace) ulisp_panic("failed to allocate Workspace");
    first_object = &Workspace[0].objects[0];
    last_object = &Workspace[NUM_CHUNKS - 1].objects[CHUNK_SIZE - 1];
    for (ssize_t c = NUM_CHUNKS - 1; c >= 0; c--) {
        auto chunk = &Workspace[c];
        chunk->in_use = false;
        chunk->next = FreeChunks;
        FreeChunks = chunk;
    }
    Freespace = NUM_CHUNKS * CHUNK_SIZE;
}

inline bool is_an_object(object* x) {
    if (x == nil) return true;
    if (x >= first_object && x <= last_object) {
        // If it's in bounds at all, check if it's in the valid portion of a chunk
        return ((x - first_object) % sizeof(chunk_t)) / sizeof(object) < CHUNK_SIZE;
    }
    return false;
}

inline bool consp(object* x) {
    return x != nil && is_an_object(car(x)) && is_an_object(cdr(x));
}

inline bool listp(object* x) {
    return x == nil || consp(x);
}

object* myalloc(object_type* type) {
    if (type && type->abstract_type) {
        ulisp_panic("attempted to create an object with an abstract type");
    }
    if (Freespace == 0) {
        error2("out of memory");
    }
    if (FreelistLength == 0) {
        auto next_chunk = FreeChunks;
        FreeChunks = FreeChunks->next;
        next_chunk->in_use = true;
        auto objects = next_chunk->objects;
        for (ssize_t i = CHUNK_SIZE - 1; i >= 0; i--) {
            auto obj = &objects[i];
            car(obj) = nil;
            cdr(obj) = Freelist;
            Freelist = obj;
            FreelistLength++;
        }
    }
    auto obj = Freelist;
    Freespace--;
    FreelistLength--;
    obj->typeinfo = type;
    Freelist = cdr(obj);
    cdr(obj) = car(obj) = nil;
    return obj;
}

object* cons(object* x, object* y) {
    auto object = myalloc(NULL);
    car(object) = x;
    cdr(object) = y;
    return object;
}

void markobject(object* obj) {
    mark:
    if (obj == nil || marked(obj)) return;
    auto head = car(obj);
    auto type = obj->typeinfo;
    mark(obj);
    if (is_an_object(head)) {
        markobject(head);
        obj = cdr(obj);
        goto mark;
    }
    if (type != NULL) {
        auto markfunc = getmarkmethod(obj);
        obj = markfunc ? markfunc(obj) : cdr(obj);
        goto mark;
    }
}

static void sweep() {
    Freelist = nil;
    Freespace = 0;
    FreelistLength = 0;
    FreeChunks = NULL;
    for (ssize_t c = NUM_CHUNKS - 1; c >= 0; c--) {
        auto chunk = &Workspace[c];
        auto objects = chunk->objects;
        auto freelist_here = Freelist;
        auto freelist_length_here = FreelistLength;
        bool any_used = false;
        if (!chunk->in_use) {
            Freespace += CHUNK_SIZE;
            goto unused_chunk;
        }
        for (ssize_t i = CHUNK_SIZE - 1; i >= 0; i--) {
            auto obj = &objects[i];
            if (marked(obj)) {
                unmark(obj);
                any_used = true;
            } else {
                car(obj) = _OBJ_IN_FREELIST_TYPE;
                cdr(obj) = freelist_here;
                Freespace++;
                FreelistLength++;
            }
        }
        if (!any_used) {
            Freelist = freelist_here;
            FreelistLength = freelist_length_here;
            chunk->in_use = false;
            unused_chunk:
            chunk->next = FreeChunks;
            FreeChunks = chunk;
        }
    }
}

void gc(object* obj1, object* obj2) {
    markobject(obj1);
    markobject(obj2);
    markobject(GCStack);
    markobject(GlobalEnv);
    markobject(tee);
    // Call all of the mark functions for globals n' stuff
    iter_modules<void*, void*>(NULL, [](native_module_t* module, void* arg, bool* stop) -> void* {
        (void)arg;
        if (module->pre_garbage_collect) module->pre_garbage_collect();
        return NULL;
    });
    sweep();
}

template<typename T, typename U>
U iter_objects(T arg, U (*callback)(object* obj, T arg, bool* stop)) {
    bool stop = false;
    object* result = nil;
    for (size_t c = 0; stop && c < NUM_CHUNKS; c++) {
        auto chunk = &Workspace[c];
        if (!chunk->in_use) continue;
        auto objects = chunk->objects;
        for (size_t i = 0; stop && i < CHUNK_SIZE; i++) {
            auto obj = &objects[i];
            if (car(obj) != _OBJ_IN_FREELIST_TYPE) {
                result = callback(obj, arg, &stop);
            }
        }
    }
    return result;
}
