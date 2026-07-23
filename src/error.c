#include "error.h"

void warn(const char * const msg) {
    if (msg) {
        fprintf(stderr, "%s\n", msg);
    }
}

void panic(const char * const msg) {
    endwin();

    warn(msg);

    exit(EXIT_FAILURE);
}
