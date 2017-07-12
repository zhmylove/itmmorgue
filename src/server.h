// vim: sw=4 ts=4 et :
#ifndef SERVER_H
#define SERVER_H

#include "itmmorgue.h"
#include "config.h"
#include "protocol.h"

#define SERVER_PORT 2607
#define SERVER_BACKLOG 8

typedef struct processor_args {
    struct sockaddr_in curr_client;
    socklen_t curr_client_len;
    int socket;
    int id;
    mqueue_t *mqueueptr;
} processor_args_t;

int server_started;

void server();
void server_fork_start();

void* process_client(processor_args_t *pargs);

#endif /* SERVER_H */
