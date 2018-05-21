// vim: et sw=4 ts=4 :

#include "bt_actions.h"

#include "entity.h"

#include "stdio.h"

#define EMPTY_LEAF_INIT_FUNCTION(f) \
    void f##_init(entity_t *a, void *b, void*c) {(void)a,(void)b,(void)c;}


/*
 * Tell Current Node Action
 */

EMPTY_LEAF_INIT_FUNCTION(tell_current_node)

enum bt_node_status tell_current_node(entity_t* e, void* context) {
    (void)context;
#ifdef _DEBUG
    printf("%s\n", ((struct creature_context*)e->context)->bt_current->name);
#endif
    return BT_SUCCESS;
}

/*
 * Tell Current Node Action (with BT_FAILURE)
 */

EMPTY_LEAF_INIT_FUNCTION(tell_current_node_fail)

enum bt_node_status tell_current_node_fail(entity_t* e, void* context) {
    (void)context;
#ifdef _DEBUG
    printf("%s\n", ((struct creature_context*)e->context)->bt_current->name);
#endif
    return BT_FAILURE;
}

/*
 * Move Around Square Action
 */

void square_move_init(entity_t *entity, void* context, void* init_data) {
    struct square_move_context * ctx = (struct square_move_context *)context;

    ctx->ul_corner.y = entity->y;
    ctx->ul_corner.x = entity->x;
    ctx->side_length = 7;
    ctx->direction = (random() % 2) ? ROT_CW : ROT_CCW;
    ctx->step_direction = DIR_E;

    // TODO fill properly (maybe)
    (void)init_data;
    //*(struct square_move_context*)context = *(struct square_move_context*)init_data;
}

enum bt_node_status square_move(entity_t* e, void* context) {
    struct square_move_context* ctx = (struct square_move_context*) context;

#define X e->x
#define Y e->y
#define UL_X ctx->ul_corner.x
#define UL_Y ctx->ul_corner.y
#define SQ_LEN ctx->side_length
#define DIR ctx->step_direction

    int flag = 0;
    switch (ctx->direction) {
        case ROT_CW:
            do{
                switch (DIR) {
                    case DIR_E:
                        if (X >= UL_X + SQ_LEN) { DIR = DIR_S; continue; }
                        X++;
                        break;
                    case DIR_S:
                        if (Y >= UL_Y + SQ_LEN) { DIR = DIR_W; continue; }
                        Y++;
                        break;
                    case DIR_W:
                        if (X <= UL_X) { DIR = DIR_N; continue; }
                        X--;
                        break;
                    case DIR_N:
                        if (Y <= UL_Y) { DIR = DIR_E; continue; }
                        Y--;
                        break;
                    default:
                        break;
                    }
                flag = 1;
            } while(!flag);
            break;

        case ROT_CCW:
            do{
                switch (DIR) {
                    case DIR_S:
                        if (Y >= UL_Y + SQ_LEN) {
                            DIR = DIR_E;
                            continue;
                        }
                        Y++;
                        break;

                    case DIR_E:
                        if (X >= UL_X + SQ_LEN) {
                            DIR = DIR_N;
                            continue;
                        }
                        X++;
                        break;

                    case DIR_N:
                        if (Y <= UL_Y) {
                            DIR = DIR_W;
                            continue;
                        }
                        Y--;
                        break;

                    case DIR_W:
                        if (X <= UL_X) {
                            DIR = DIR_S;
                            continue;
                        }
                        X--;
                        break;
                    default:
                        break;
                }
                flag = 1;
            } while(!flag);
            break;
    }

#undef X
#undef Y
#undef UL_X
#undef UL_Y
#undef SQ_LEN
#undef DIR

    return BT_RUNNING;
}

#undef EMPTY_LEAF_INIT_FUNCTION
