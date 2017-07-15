// vim: sw=4 ts=4 et :
#ifndef CONNECTION_H
#define CONNECTION_H

/*
 * Represents client's connection to server.
 */
typedef struct connection {
    struct sockaddr_in curr_client; // Client socket address
    socklen_t curr_client_len;      // Size of previous field
    int socket;                     // Client socket file descriptor
    pthread_t thread;               // Worker thread
    int id;                         // Number of client
    uint32_t sysmsg_mask;           // Mask of system messages (see protocol.h)
    mqueue_t *mqueueptr;            // Message queue for current client
    struct connection *prev;        // Previous client
    struct connection *next;        // Next client
} connection_t;

#endif /* CONNECTION_H */
