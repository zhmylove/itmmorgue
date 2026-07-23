#include "npc.h"

npc_t npcs[2048];
size_t npcs_len = 0;

void s_send_npc(connection_t * connection, npc_t * npc) {
    npc_mbuf_t * npc_mbuf;

    if (!connection) {
        return;
    }

    if (!(npc_mbuf = (npc_mbuf_t *) malloc(sizeof(npc_mbuf_t)))) {
        panic("Error creating npc_mbuf to send!");
    }

    npc_mbuf->id = npc->id;

    npc_mbuf->tile = npc->tile;
    npc_mbuf->color = npc->color;

    npc_mbuf->x = npc->x;
    npc_mbuf->y = npc->y;

    mbuf_t s2c_mbuf;
    s2c_mbuf.payload = (void *) npc_mbuf;
    s2c_mbuf.msg.type = MSG_PUT_NPC;
    s2c_mbuf.msg.size = sizeof(npc_mbuf_t);

    loggerf("[S] Sending NPC id: %u", npc->id);
    mqueue_put(connection->mqueueptr, s2c_mbuf);
}

void c_receive_npc(npc_mbuf_t * mbuf) {
    if (!mbuf) {
        return;
    }

    if (npcs_len < mbuf->id + 1) {
        npcs_len = mbuf->id + 1;
    }

    npcs[mbuf->id].tile = mbuf->tile;
    npcs[mbuf->id].color = mbuf->color;

    npcs[mbuf->id].x = mbuf->x;
    npcs[mbuf->id].y = mbuf->y;
}

