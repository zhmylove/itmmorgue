// vim: et sw=4 ts=4 :

#include "bt.h"

#include "entity.h"

#include "npc.h"
#define BT_ARRAY
#include "npc.h"
#undef BT_ARRAY

#include <stddef.h>

static inline int bt_init_tree(bt_root_t*);

int bt_init(){
    for( bt_root_t** p = bt_trees; NULL != *p; ++p)
        bt_init_tree(*p);
}

/*
 * Traverse a behaviour tree (*root): calculate context_size and fill pointers
 *
 * root : root of a Behavior Tree
 */

static inline int bt_init_tree(bt_root_t* root){
    int stack[BT_MAX_NESTED_NODES] = {0};
    int stack_idx = 0;

    bt_node_t* current = root->child;
    bt_node_t* child = NULL;
    size_t context_size = 0;

    context_size += sizeof(struct bt_root_context);

    #define UP()    do {                           \
                        current = current->parent; \
                        stack[stack_idx--] = 0;    \
                    } while(0)
    #define DOWN()  do {                           \
                        child->parent = current;   \
                        current = child;           \
                        stack[stack_idx++]++;      \
                    } while(0)
    #define C_CHILD(c,i) (c)->u.composite.children[(i)]
    #define D_CHILD(c) (c)->u.decorator.child

    do {
        switch( current->type ){
            case _BT_COMPOSITE:
                child = C_CHILD(current, stack[stack_idx]);

                // child == NULL => traversed all childs - return to parent
                if( NULL == child ){
                    UP();
                    break;
                }
                // Allocate context for composite if visit node first time
                if( 0 == stack[stack_idx] ){
                    current->u.composite.offset = context_size;
                    context_size += sizeof(struct bt_composite_context);
                }

                DOWN();
                #undef CHILD
                break;

            case _BT_DECORATOR:
                // Decorator has only 1 child => stack value > 0 = backtracking
                if( stack[stack_idx] > 0 ){
                    UP();
                    break;
                }
                child = D_CHILD(current);
                DOWN();
                break;

            case _BT_LEAF:;
                size_t offset = context_size;
                context_size += current->u.leaf.u.context_size;
                current->u.leaf.u.offset = offset;
                UP();
                break;
        }
    // Returned to the root.child after traversing the whole tree
    } while( stack_idx != -1 );

    #undef UP
    #undef DOWN
    #undef C_CHILD
    #undef D_CHILD

    root->context_size = context_size;
    return 0;
}

#define OFFSET(p, o) (((char*)(p))+(o))

/*
 * Get BT context from entity
 * e : entity_t* entity that owns requested BT context
 */
#define GET_BT_CTX(e) \
((struct creature_context*) e->context)->bt_context

/*
 * Get Root context from BT Context
 * g : void* BT context
 */
#define GET_ROOT_CTX(g) \
((struct bt_root_context*) OFFSET(g, BT_ROOT_CONTEXT_OFFSET ))

/*
 * Get Leaf Context
 * g : void* BT context
 * n : bt_node_t* node (only Leafs)
 */
#define GET_LEAF_CTX(g, n) \
((void*) OFFSET(g, n->u.leaf.u.offset ))

/*
 * Get Composite Context
 * g : void* BT context
 * n : bt_node_t* node (only Leafs)
*/
#define GET_COMPOSITE_CTX(g,n) \
((struct bt_composite_context*) (OFFSET(g, n->u.composite.offset )))

/*
 * Execute BT for entity_t->type of CREATURE
 * e : entity behaviour of which is to be executed
 */
static inline int bt_execute_creature(entity_t* e){
    struct creature_context* cr_ctx = (struct creature_context*) e->context;
    bt_root_t* root = cr_ctx->bt_root;
    bt_node_t* current = cr_ctx->bt_current;
    bt_node_t* child = NULL;
    void* bt_ctx = GET_BT_CTX(e);

    enum bt_node_status* last_status_p = &GET_ROOT_CTX(bt_ctx)->last_status;

    #define UP() current = current->parent

// 0. Rerun if did not finish

    // In this implementation, BT_RUNNING can only be returned by Leaf
    // BT_RUNNING means that current node is Leaf - just execute again
    if( BT_RUNNING == *last_status_p )
        goto stage_execute;

    #define SET_STATUS(s) *last_status_p = (s)

// 1. Walk up to the first node with unfinished execution
stage_up:
    // No check for BT_RUNNING - see stage 0 (rerun).
    do{
        switch( current->type ){
            case _BT_COMPOSITE:;
                int* child_idx_p = &GET_COMPOSITE_CTX(
                    bt_ctx,
                    current
                )->next_child_idx;
                child = current->u.composite.children[*child_idx_p];
                if( NULL == child ){
                    *child_idx_p = 0;
                    UP();
                    break;
                }
                switch(current->u.composite.type){
                    case _BT_SEQUENCE:
                        // No children failed yet - Continue exec of Sequence
                        if( BT_SUCCESS == *last_status_p ){
                            goto stage_down;
                        }
                        SET_STATUS(BT_FAILURE);
                        break;

                    case _BT_SELECTOR:
                        // Found first successful child - return to parent
                        if( BT_SUCCESS == *last_status_p ){
                            // SET_STATUS(BT_SUCCESS); // already set
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
                switch(current->u.decorator.type){
                    case _BT_NOT:
                        SET_STATUS( !*last_status_p );
                        UP();
                        break;

                    case _BT_UNTIL_FAILURE:
                        if( BT_FAILURE != *last_status_p )
                            goto stage_down;
                        // SET_STATUS( BT_SUCCESS );
                        // UP();

                    /* FALLTHRU */
                    case _BT_SUCCEEDER:
                        SET_STATUS(BT_SUCCESS);
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
    } while( current != NULL ); // NULL = reached bt_root - restart behaviour

// Only reached if current is NULL
// gotoed to stage_down normally
current = root->child;
#undef UP
#undef SET_STATUS

#define DOWN() current = child

// 2. Walk down to executable Leaf
stage_down:
    do{
        switch(current->type){
            case _BT_COMPOSITE:;
                // For now, it's impossible for an untouched/finished composite
                // to have NULL as a current child. Hence, no checks for NULL
                int* child_idx_p = &GET_COMPOSITE_CTX(
                    bt_ctx,
                    current
                )->next_child_idx;
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
    } while( current->type != _BT_LEAF );

// 3. Execute a leaf
stage_execute:
    cr_ctx->bt_current = current;
    leaf_function_t leaf_exec = current->u.leaf.function;
    *last_status_p = leaf_exec(e, GET_LEAF_CTX(bt_ctx, current));
    return 0;
#undef DOWN
}

int bt_execute(entity_t* e){
    switch(e->type){
        case CREATURE:;
            return bt_execute_creature(e);

        case OBJECT:
        case PLAYER:
            return 0;

        default:
            return 1;
    }
}