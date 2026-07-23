#include "npc_traits.h"

void npc_trait_square_movement(npc_t * npc, void * ctx)
{
    // ctx is for external context, not bound to current NPC
    (void) ctx;

    if (!npc) {
        return;
    }

    trait_square_movement_ctx_t * trait_ctx = (trait_square_movement_ctx_t *) npc->trait_ctx;
    switch (trait_ctx->direction) {
        case TOP:
            if (trait_ctx->distance == (trait_ctx->progress)++) {
                trait_ctx->direction = RIGHT;
                trait_ctx->progress = 0;
            }
            --(npc->y);
            break;
        case RIGHT:
            if (trait_ctx->distance == (trait_ctx->progress)++) {
                trait_ctx->direction = DOWN;
                trait_ctx->progress = 0;
            }
            ++(npc->x);
            break;
        case DOWN:
            if (trait_ctx->distance == (trait_ctx->progress)++) {
                trait_ctx->direction = LEFT;
                trait_ctx->progress = 0;
            }
            ++(npc->y);
            break;
        case LEFT:
            if (trait_ctx->distance == (trait_ctx->progress)++) {
                trait_ctx->direction = TOP;
                trait_ctx->progress = 0;
            }
            --(npc->x);
            break;
    }
}
