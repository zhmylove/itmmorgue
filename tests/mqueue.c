// vim: sw=4 ts=4 et :
#include <unistd.h>
#include <stdio.h>
#include "protocol.h"

/*
 * cc -o tests/mqueue tests/mqueue.c -I src/ -Wall -Wextra --std=gnu99 \
 *     src/protocol.c  -I lib/ && tests/mqueue
 */

void panic(char *str) {
    fprintf(stderr, "Caught panic: %s", str);
    _exit(2);
}

int main() {   
    mqueue_t mq;

    mqueue_init(&mq);

    mbuf_t mbuf;

    if (mqueue_get(&mq, &mbuf) != 0) {
        fprintf(stderr, "FAIL: get from zero-sized queue!");
        return 1;
    }

    for (size_t i = 0; i < MQUEUE_SIZE - 1; i++) {
        mbuf.payload = NULL;
        mbuf.msg.size = i;
        mqueue_put(&mq, mbuf);
    }

    for (size_t i = 0; i < MQUEUE_SIZE - 1; i++) {
        if (mqueue_get(&mq, &mbuf) != 1) {
            fprintf(stderr, "FAIL: get from queue!");
        }

        if (mbuf.msg.size != i) {
            fprintf(stderr, "FATAL: corrupted data!");
        }
    }

    return 0;
}
