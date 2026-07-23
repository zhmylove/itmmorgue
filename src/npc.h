#ifndef NPC_H
#define NPC_H

#include "itmmorgue.h"

struct npc;

typedef void (*npc_trait_t)(struct npc * npc, void * ctx);

typedef struct npc {
    uint32_t id;

    enum stuff tile;
    enum colors color;

    uint16_t x;
    uint16_t y;

    /* npc stats (hp, mp, atk, def, (de)buffs, speed...) */

    void * trait_ctx;
    npc_trait_t trait;
} npc_t;

/* currently, the only difference between partial and full
   NPC update is tile and color, so there are no partials */
typedef struct npc_mbuf {
    uint32_t id;

    enum stuff tile;
    enum colors color;

    uint16_t x;
    uint16_t y;
} npc_mbuf_t;

void s_send_npc(connection_t * connection, npc_t * npc);
void c_receive_npc(npc_mbuf_t * mbuf);

/* should use some data structure with ability to query
   by coords (x, y) */
extern npc_t npcs[];
extern size_t npcs_len;

#endif // #ifndef NPC_H
