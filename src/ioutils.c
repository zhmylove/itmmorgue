#include "ioutils.h"


int synchronized_readall(
        pthread_mutex_t *mutex,
        int fd,
        void *buf,
        size_t size
) {
    if (NULL == mutex) {
        panic("synchronized_readall: NULL mutex provided");
    }

    pthread_mutex_lock(mutex);
    int retval = readall(fd, buf, size);
    pthread_mutex_unlock(mutex);
    return retval;
}

int readall(int fd, void *buf, size_t size) {
    int rc = -1;
    size_t got = 0;

    if (size == 0) {
        return 0;
    }

    while (got < size) {
        if ((rc = read(fd, (char *)buf + got, size - got)) > 0) {
            got += rc;
        } else {
            break;
        }
    }

    if (got == size) {
        return size;
    }

    return -1;
}

