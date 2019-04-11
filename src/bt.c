// vim: et sw=4 ts=4 :

/*
 * Behaviour Tree - a structure used to define a behaviour of an NPC
 * This file contains implementation of functions on BTs.
 * All functions are used internally, except:
 *   - s_bt_init  - init all BTs
 *   - bt_execute - execute BT
 * which are to be called externally.
 */

#include "bt.h"
#include "entity.h"

#include "npc.h"
#define BT_ARRAY
#include "npc.h"
#undef BT_ARRAY

#include <stddef.h>

static inline int bt_init_tree(bt_root_t*);
static inline int bt_execute_creature(entity_t* e);

void s_bt_init() {
    for (bt_root_t** p = bt_trees; NULL != *p; ++p)
        bt_init_tree(*p);
}

int bt_execute(entity_t* e) {
    if (e == NULL) {
        return 1;
    }

    switch (e->type) {
        case CREATURE:;
            return bt_execute_creature(e);
        /* FALLTHRU */
        case OBJECT:
        case PLAYER:
            return 0;
        default:
            return 1;
    }
}

/*
 * Traverse a behaviour tree (*root): calculate context_size and fill pointers
 *
 * root : root of a Behavior Tree
 */
static inline int bt_init_tree(bt_root_t* root) {
    int stack[BT_MAX_NESTED_NODES] = {0};
    int stack_idx = 0;

    bt_node_t* current = root->child;
    bt_node_t* child = NULL;
    size_t context_size = 0;

    context_size += sizeof(struct bt_root_context);

    #define UP()    do {                           \
                        current = current->parent; \
                        stack[stack_idx--] = 0;    \
                    } while (0)
    #define DOWN()  do {                           \
                        child->parent = current;   \
                        current = child;           \
                        stack[stack_idx++]++;      \
                    } while (0)

    size_t offset;
    do {
        switch (current->type) {
            case _BT_COMPOSITE:
                child = current->u.composite.children[stack[stack_idx]];
                // child == NULL => traversed all childs - return to parent
                if (NULL == child) {
                    UP();
                    break;
                }
                // Allocate context for composite if visit node first time
                if (0 == stack[stack_idx]) {
                    current->u.composite.offset = context_size;
                    context_size += sizeof(struct bt_composite_context);
                }

                DOWN();
                #undef CHILD
                break;
            case _BT_DECORATOR:
                // Decorator has only 1 child => stack value > 0 = backtracking
                if (stack[stack_idx] > 0) {
                    UP();
                    break;
                }
                child = current->u.decorator.child;
                DOWN();
                break;
            case _BT_LEAF:
                offset = context_size;
                context_size += current->u.leaf.u.context_size;
                current->u.leaf.u.offset = offset;
                UP();
                break;
        }
    // Returned to the root.child after traversing the whole tree
    } while (stack_idx != -1);

    #undef UP
    #undef DOWN

    root->context_size = context_size;
    return 0;
}

#define OFFSET(p, o) (((char*)(p))+(o))

/*
 * Execute BT for entity_t->type of CREATURE
 * e : entity behaviour of which is to be executed
 */
static inline int bt_execute_creature(entity_t* e) {
    struct creature_context* cr_ctx = (struct creature_context*) e->context;
    bt_root_t* root = cr_ctx->bt_root;
    bt_node_t* current = cr_ctx->bt_current;
    bt_node_t* child = NULL;
    void* bt_ctx = ((struct creature_context*) e->context)->bt_context;

    enum bt_node_status* last_status_p =
        &( (struct bt_root_context*)
            OFFSET(bt_ctx, BT_ROOT_CONTEXT_OFFSET)
        )->last_status;
    enum bt_node_status last_status = *last_status_p;

    if (NULL == current)
        current = cr_ctx->bt_current = root->child;

#define UP() current = current->parent

/*
 * Stage 0: Rerun if did not finish
 */

    // In this implementation, BT_RUNNING can only be returned by Leaf
    // BT_RUNNING means that current node is Leaf - just execute again
    if (BT_RUNNING == *last_status_p)
        goto stage_execute;

/*
 * Stage 1: Walk up to the first node with unfinished execution
 */

    // No check for BT_RUNNING - see stage 0 (rerun).
    int* child_idx_p;
    do {
        switch (current->type) {
            case _BT_COMPOSITE:
                child_idx_p =
                    &((struct bt_composite_context*)
                        OFFSET(bt_ctx, current->u.composite.offset)
                    )->next_child_idx;
                child = current->u.composite.children[*child_idx_p];
                if (NULL == child) {
                    *child_idx_p = 0;
                    UP();
                    break;
                }
                switch (current->u.composite.type) {
                    case _BT_SEQUENCE:
                        // No children failed yet - Continue exec of Sequence
                        if (BT_SUCCESS == *last_status_p) {
                            goto stage_down;
                        }
                        last_status = BT_FAILURE;
                        break;
                    case _BT_SELECTOR:
                        // Found first successful child - return to parent
                        if (BT_SUCCESS == *last_status_p) {
                            // last_status = BT_SUCCESS; // already set
                            break;
                        }
                        goto stage_down;
                };
                // Clean up composite context
                *child_idx_p = 0;
                // Reached this line - composite is still in execution
                UP();
                continue;
            case _BT_DECORATOR:
                switch (current->u.decorator.type) {
                    case _BT_NOT:
                        last_status = !*last_status_p;
                        UP();
                        break;

                    case _BT_UNTIL_FAILURE:
                        if( BT_FAILURE != *last_status_p )
                            goto stage_down;
                        // last_status = BT_SUCCESS;
                        // UP();

                    /* FALLTHRU */
                    case _BT_SUCCEEDER:
                        last_status = BT_SUCCESS;
                        UP();
                        break;

                    // TODO: implement registration of event-handlers
                    // case _BT_EVENT_HANDLER:

                    default:
                        return 1;
                }
                continue;
            case _BT_LEAF:
                UP();
                continue;
        }
    } while (current != NULL); // NULL = reached bt_root - restart behaviour

    // Only reached if current is NULL
    // gotoed to stage_down normally
    current = root->child;
#undef UP

/*
 * Stage 2: Walk down to executable Leaf
 */

#define DOWN() current = child
stage_down:
    do{
        switch (current->type) {
            case _BT_COMPOSITE:
                // For now, it's impossible for an untouched/finished composite
                // to have NULL as a current child. Hence, no checks for NULL
                child_idx_p = &((struct bt_composite_context*) OFFSET(
                                    bt_ctx, current->u.composite.offset
                                ))->next_child_idx;
                child = current->u.composite.children[*child_idx_p];
                (*child_idx_p)++;
                DOWN();
                break;
            case _BT_DECORATOR:
                child = current->u.decorator.child;
                DOWN();
                break;
            case _BT_LEAF:
                break;
        }
    } while ( current->type != _BT_LEAF );

/*
 * Stage 3: Execute a leaf
 */

stage_execute:
    cr_ctx->bt_current = current;
    leaf_function_t leaf_exec = current->u.leaf.function;
    last_status = leaf_exec(
        e,
        (void*) OFFSET(bt_ctx, current->u.leaf.u.offset)
    );
    *last_status_p = last_status;
    return 0;
#undef DOWN
}

#undef OFFSET
