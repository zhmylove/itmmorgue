// vim: sw=4 ts=4 et :
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "itmmorgue.h"

typedef struct msg_t {
    enum msg_type {
        MSG_ECHO_REQUEST,   // s2c echo request
        MSG_ECHO_REPLY,     // c2s echo reply
        MSG_GET_AREA,       // c2s area request
        MSG_PUT_AREA,       // s2c area transmission
        MSG_GET_CHAT,       // c2s chat history request
        MSG_NEW_CHAT,       // c2s chat new message
        MSG_PUT_CHAT,       // s2c chat history update
    } type;
    size_t size;
    char payload[1];
} msg_t;

#endif /* PROTOCOL_H */
