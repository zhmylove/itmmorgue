// vim: et sw=4 ts=4 :

#include "bt.h"

#include "entity.h"

#include <stddef.h>


/*
 * Traverse a behaviour tree (*root): calculate context_size and fill pointers
 *
 * root : root of a Behavior Tree
 */

int bt_init(bt_root_t* root){
    int stack[BT_MAX_NESTED_NODES] = {0};
    int stack_idx = 0;

    bt_node_t* current = root->child;   
    bt_node_t* child = NULL;
    size_t context_size=0;

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