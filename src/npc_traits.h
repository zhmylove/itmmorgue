#ifndef NPC_TRAITS_H
#define NPC_TRAITS_H

#include "npc.h"

typedef struct trait_square_movement_ctx {
    enum {
        TOP = 0,
        RIGHT,
        DOWN,
        LEFT
    } direction;
    size_t distance, progress;
} trait_square_movement_ctx_t;

void npc_trait_square_movement(npc_t * npc, void * ctx);

#endif // #ifndef NPC_TRAITS_H
