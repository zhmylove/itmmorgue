// vim: sw=4 ts=4 et :
#ifndef CONNECTION_H
#define CONNECTION_H

typedef struct connection {
    struct sockaddr_in curr_client;
    socklen_t curr_client_len;
    int socket;
    int id;
    mqueue_t *mqueueptr;
} connection_t;

#endif /* CONNECTION_H */
