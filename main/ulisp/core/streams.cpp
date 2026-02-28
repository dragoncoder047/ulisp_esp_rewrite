#include <stdint.h>
#include <stdio.h>
#include "streams.hpp"
#include "macros.hpp"

/*
The actual in-memory uLisp stream object looks like this:
     stream object
          |
          v
+---------------------+----+        +-------+-------+
| pointer to "stream" |    |        |       |       | <--------the cdr pointer is set to NIL
|    ulisp_type_t     |  o--------->|   o   |   o   |          after the stream is closed
+---------------------+----+        +---|---+---|---+
                                        |       '-----------------------.
                                        v                               |
                                   +--------+---------------------+     |
                                   |  NULL  | pointer to relevant |     |
                                   |        |  streamtbl_entry_t  |     |
                                   +--------+---------------------+     |
                                                                        |
.-----------------------------------------------------------------------'
|
|   +---------+-------+      +-------------------------------+
|   |         |       |      |   t if last printed is \n     |
'-->|    o    |   o--------->| nil if last printed is not \n |
    +----|----+-------+      +-------------------------------+
         |                                  ^
         |                                  |
         |                      this is used to implement (fresh-line)
         |                             and the format code ~&
         v
    +----------+----------------------------+
    |   NULL   |  pointer to FILE* pointer  |
    |          |  or NULL if it's closed    |
    +----------+----------------------------+

The streams would be protected and automatically fclose()'d when GC'ed by making sure that the stream typed object is GC'ed first,
and then the rest of the stream payload. This is accomplished by the core extension's before_gc_hook function
which here, would find all live stream objects and mark their payloads (but not the object cell themselves) so to make sure
the stream can be closed properly before the payload is destroyed by the garbage collector.

To determine whether a cell is a cons or a special type, check if the car and cdr pointers point to valid Workspace memory locations.
If the car is out-of-bounds, but is not NULL, it is assumed it points to a typetbl_entry_t. If the car is NULL and the cdr
is out-of-bounds it is assumed it is just a "wild pointer" that means nothing to uLisp and is likely from an extension.
Wild pointers would print out like <pointer:0x00df54a8> or something like that.
*/

inline bool streamopenp(object* stream) {
    return !!cddr(stream);
}
