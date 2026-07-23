#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <unistd.h>
#include <pthread.h>
#include "error.h"

int readall(int fd, void *buf, size_t size);

int synchronized_readall(
        pthread_mutex_t *mutex,
        int fd,
        void *buf,
        size_t size
);

#endif /* IO_UTILS_H */
