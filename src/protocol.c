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

void mqueue_destroy(mqueue_t *queue) {
    pthread_mutex_destroy(&queue->mutex);
}

/*
 * Writes mbuf with non-zero payload size into socket.
 *
 * mbuf   : buffer to send
 * socket : receiver
 *
 * ret    : -1 on failure
*/
static int send_fat_mbuf( int socket, mbuf_t *mbuf) {
    struct iovec parts[2];
    parts[0].iov_base = (caddr_t)&mbuf->msg;
    parts[0].iov_len = sizeof(mbuf->msg);
    parts[1].iov_base = mbuf->payload;
    parts[1].iov_len = mbuf->msg.size;
    int rc = writev(socket, parts, 2);

    free(mbuf->payload);

    return rc;
}

/*
 * Writes any mbuf size into socket.
 *
 * mbuf   : buffer to send
 * socket : receiver
 *
 * ret    : -1 on failure
*/
int send_mbuf(int socket, mbuf_t *mbuf) {
    if (mbuf->msg.size == 0) {
        return write(socket, &mbuf->msg, sizeof(msg_t));
    }
    return send_fat_mbuf(socket, mbuf);
}
