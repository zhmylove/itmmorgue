// vim: sw=4 ts=4 et :
#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifndef PROTOCOL_VERSION
#define PROTOCOL_VERSION 0x2
#endif /* PROTOCOL_VERSION */

#define MQUEUE_SIZE 128

typedef struct msg {
    enum msg_type {
        MSG_ECHO_REQUEST,   // s2c echo request
        MSG_ECHO_REPLY,     // c2s echo reply
        MSG_GET_AREA,       // c2s area request
        MSG_PUT_AREA,       // s2c area transmission
        MSG_GET_CHAT,       // c2s chat history request
        MSG_NEW_CHAT,       // c2s chat new message
        MSG_PUT_CHAT,       // s2c chat history update
        MSG_PUT_SYS,        // s2c sysmsg history update
    } type;
    int version;
    size_t size;
} msg_t;

typedef struct mbuf {
    msg_t msg;
    void *payload;
} mbuf_t;

typedef struct mqueue {
    mbuf_t buf[MQUEUE_SIZE];
    size_t start_position;
    size_t size;
} mqueue_t;

int mqueue_get(mqueue_t *queue, mbuf_t *mbuf);
void mqueue_put(mqueue_t *queue, mbuf_t mbuf);
void mqueue_init(mqueue_t *queue);

#endif /* PROTOCOL_H */
