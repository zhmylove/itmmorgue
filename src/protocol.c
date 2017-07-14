// vim: sw=4 ts=4 et :
#include "itmmorgue.h"

void mqueue_init(mqueue_t *queue) {
    if (queue == NULL) {
        panic("Trying to initialize NULL mqueue!");
    }

    queue->size = 0;
    queue->start_position = 0;
    if (0 != pthread_mutex_init(&queue->mutex, NULL)) {
        panic("Cannot initialize message queue mutex!");
    }
}

void mqueue_put(mqueue_t *queue, mbuf_t mbuf) {
    if (queue == NULL) {
        panic("Trying to put in NULL mqueue!");
    }

    int rc;
    if (0 != (rc = pthread_mutex_lock(&queue->mutex))) {
        if (EDEADLK == rc) {
            panic("Message queue: deadlock!");
        }
        panic("Message queue: failure during mutex locking");
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

    mbuf.msg.version = PROTOCOL_VERSION;
    queue->buf[pos] = mbuf;
    queue->size++;

    pthread_mutex_unlock(&queue->mutex);
}

int mqueue_get(mqueue_t *queue, mbuf_t *mbuf) {
    if (queue == NULL) {
        panic("Trying to get from NULL mqueue!");
    }

    int rc;
    if (0 != (rc = pthread_mutex_lock(&queue->mutex))) {
        if (EDEADLK == rc) {
            panic("Message queue: deadlock!");
        }
        panic("Message queue: failure during mutex locking");
    }

    if (queue->size == 0) {
        pthread_mutex_unlock(&queue->mutex);
        return 0;
    }

    *mbuf = queue->buf[queue->size--, queue->start_position++];
    if (queue->start_position >= MQUEUE_SIZE) {
        queue->start_position = 0;
    }

    pthread_mutex_unlock(&queue->mutex);
    return 1;
}

