#pragma once
#include "types.hpp"
typedef struct stream_kind_t stream_kind_t;
struct stream_kind_t {
    // The "kind" of the stream (displayed when printing the stream)
    const char* const name;
    // The prefix that (open) uses to find the stream opener
    const char* const prefix;
    // Called when the stream is opened in a (with-X) block or via (open)
    // -- path is a pointer to the path after the prefix,
    //    e.g for (open "/sd/foo.txt" :mode :write-append) if the prefix is "/sd" the path will be "/foo.txt"
    // -- args is the "mode" arguments passed to open function
    //    e.g for (open "/dev/i2c" :address #x70 :mode :read :if-does-not-exist nil)
    //    args will be (:address #x70 :mode :read :if-does-not-exist nil)
    FILE* (*opener)(const char* const path, object* args);
};

bool streamopenp(object* stream);
