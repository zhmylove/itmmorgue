// vim: sw=4 ts=4 et :
#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifndef PROTOCOL_VERSION
#define PROTOCOL_VERSION 0x2
#endif /* PROTOCOL_VERSION */

#define MQUEUE_SIZE 128

/*
 * Describes message, sent to client. Message has its *type* (s2c and c2s
 * stands as "server to client" and "client to server" message type). *version*
 * is protocol version (TODO: check it on message receive), and *size* is
 * payload size, used to point, how many bytes receiver shall read from socket.
 */
typedef struct msg {
    enum msg_type {
        MSG_ECHO_REQUEST,     // s2c echo request
        MSG_ECHO_REPLY,       // c2s echo reply

        MSG_PUT_LEVEL,        // s2c level transmission
        MSG_PUT_AREA,         // s2c area transmission

        MSG_GET_CHAT,         // c2s chat history request
        MSG_NEW_CHAT,         // c2s chat new message
        MSG_PUT_CHAT,         // s2c chat history update

        MSG_PUT_SYSMSG,       // s2c sysmsg history update
        MSG_SUBSCRIBE_SYSMSG, // c2s sysmsg subscription mask

        MSG_PUT_STATUS,       // s2c player status update
    } type;
    int version;              // Protocol version, generated during compilation
    size_t size;              // Size of payload or zero if there is no payload

    // TODO: *int* and *size_t* are types with architecture-dependent sizes.
    // Shall we use them? I guess no. Message, sent from 64-bit machine can be
    // incorrectly interpreted on 32-bit machine because of half-sized
    // *size_t*.
} msg_t;

/*
 * Type of sysmsg.
 * Sysmsg with this TYPE should be sent to client only when
 * (SYSMSG_MASK & TYPE) is true in boolean context. Otherwise, server should
 * drop this message before sending.
 *
 * TODO: shall we make send_sysmsg() as the protocol part and do filtration
 * directly in this function?
 */
enum msg_sysmsg_type {
    SM_CHAT_NEW_MESSAGE = 0x01,    // New message in chat
    SM_PLAYER_JOINED    = 0x02,    // New player joined to the ITMMORGUE
};

typedef struct mbuf {
    msg_t msg;
    void *payload;
} mbuf_t;

typedef struct mqueue {
    mbuf_t buf[MQUEUE_SIZE];
    size_t start_position;
    size_t size;
    pthread_mutex_t mutex;
} mqueue_t;

void mqueue_init(mqueue_t *queue);
int mqueue_get(mqueue_t *queue, mbuf_t *mbuf);
void mqueue_put(mqueue_t *queue, mbuf_t mbuf);
void mqueue_destroy(mqueue_t *queue);

#endif /* PROTOCOL_H */
