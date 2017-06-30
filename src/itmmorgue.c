// vim: sw=4 ts=4 et :
#include "itmmorgue.h"

void warn(char *msg) {
    if (msg) {
        fprintf(stderr, "%s\n", msg);
    }
}

void panic(char *msg) {
    endwin();

    warn(msg);

    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    // TODO parse argv and run server / client
    // TODO parse config file

    client();

    (void)argc, (void)argv;
    return EXIT_SUCCESS;
}
