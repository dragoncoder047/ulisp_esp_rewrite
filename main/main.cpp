#include "ulisp/ulisp.hpp"
#include <fcntl.h>

void setup() {
    // TODO: mount SD card, etc
    // Enable blocking mode on stdin and stdout
    // Disable buffering on stdin (i.e. Serial)
    fcntl(fileno(stdout), F_SETFL, 0);
    fcntl(fileno(stdin), F_SETFL, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    initworkspace();
    initenv();
    // TODO: start main lisp file
}

void loop() {
    // TODO: enter REPL
}
