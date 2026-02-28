# ulisp_rewrite

This is going to be a rewrite of [uLisp-ESP](http://www.ulisp.com/show?3M#esp-version) but using raw ESP-IDF instead of Arduino.

## Goals of this project

* Modularize the uLisp sources into separate, easily-understandable files
* Add the ability to add multiple extensions
* Add the ability to add custom special object typess
* Use the ESP-IDF's builtin [`linenoise` terminal editor component](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/console.html) for the serial REPL
* Add the ability to add custom streams
    * This will be done using ESP-IDF's [virtual filesystem component](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/vfs.html) so that any and all streams can be standard C `FILE*` pointers, so this is likely not portable to other platforms.
* Add an Xtensa32 assembler, or at least just disable the ESP-IDF's memory protections, so that such thing as an assembler can be made
