// vim: sw=4 ts=4 et :
#include "itmmorgue.h"

void mqueue_init(mqueue_t *queue) {
    if (queue == NULL) {
        panic("Trying to initialize NULL mqueue!");
    }

    queue->size = 0;
    queue->start_position = 0;
}

void mqueue_put(mqueue_t *queue, mbuf_t mbuf) {
    if (queue == NULL) {
        panic("Trying to put in NULL mqueue!");
    }

    size_t pos = queue->start_position;
    size_t size = queue->size;

    if (pos >= MQUEUE_SIZE) {
        panicf("Invalid position %zu int mqueue_put", pos);
    }

    if (size == MQUEUE_SIZE - 1) {
        panic("Mqueue overflow!");
    }

    pos = (pos + size) % MQUEUE_SIZE;

    queue->buf[pos] = mbuf;
    queue->size++;
}

int mqueue_get(mqueue_t *queue, mbuf_t *mbuf) {
    if (queue == NULL) {
        panic("Trying to get from NULL mqueue!");
    }

    if (queue->size == 0) {
        return 0;
    }

    *mbuf = queue->buf[queue->size--, queue->start_position++];
    if (queue->start_position >= MQUEUE_SIZE) {
        queue->start_position = 0;
    }

    return 1;
}

