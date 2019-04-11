// vim: sw=4 ts=4 et :

/*
 * Behaviour Tree - a structure used to define a behaviour of an NPC
 * This file defines structures and functions for BTs.
 *
 * For Tree Building, it is strongly advised to use bt_macro.h,
 * designed specifically for this purpose
 */

#ifndef _BT_H_
#define _BT_H_

#include "entity_decl.h"

#include <stdlib.h>

#define BT_MAX_NESTED_NODES 100

/*
 * Status of execution for current node.
 * Defines execution of the whole BT
 */
enum bt_node_status {
    BT_SUCCESS=0,
    BT_FAILURE=1,
    BT_RUNNING
};

/* Basic BT types:
 * - Root contains information about whole tree
 * - Node contains structures that control execution of BT. Subtypes:
 *   - Composite - has 1+ children that are to be executed in certain order
 *   - Decorator - has 1 children, changes the flow of execution (e.g. repeats)
 *   - Leaf      - has NO children, what is actually executed.
 *                 (Can be condition or action,
 *                 although they differ only semantically (see BT LEAF ))
 */
typedef struct bt_root bt_root_t;
typedef struct bt_node bt_node_t;


struct bt_composite;
struct bt_decorator;
struct bt_leaf;

struct bt_root {
    size_t context_size;
    bt_node_t* child;
};

/*
 * BT_COMPOSITE
 * ------------
 * Composite is a type of Node that contains several children
 * that are to be executed in certain order. E.g.:
 *   - Execute one by one until any fails (_BT_SEQUENCE)
 *   - Execute one by one until any succeeds (_BT_SELECTOR)
 *
 * Composite has 1+ children.
 */

#define BT_ROOT_CONTEXT_OFFSET 0

struct bt_root_context {
    enum bt_node_status last_status; // Status last execution of tree returned
};

struct bt_composite {
    enum {
        _BT_SEQUENCE,       // ||
        _BT_SELECTOR        // &&
    } type;

    size_t offset;          // offset of composite's context
                            // in BT's global context (in bytes)
                            //   set in BT initialization

    bt_node_t** children;
};

struct bt_composite_context {
    int next_child_idx;
};

/*
 * BT DECORATOR
 * ------------
 * Decorator is a type of Node that defines how execution of its child
 * is to be altered. E.g.:
 *   - inverse the status they return (BT_NOT)
 *   - repeat them until they fail (BT_UNTIL_FAILURE)
 *
 * Decorator has only 1 child.
 */

struct bt_decorator {

    enum {
        _BT_NOT,           // Inversion of bt_node_status (SUCCESS <-> FAILURE)
        _BT_UNTIL_FAILURE, // Run child until FAILURE received
        _BT_SUCCEEDER,     // Returns SUCCESS even if child Fails
                           // (as usual, stalls if BT_RUNNING is recieved)

        _BT_EVENT_HANDLER  // Special Decorator for Event Handlers
    } type;

    bt_node_t* child;
};

/*
 * BT LEAF
 * -------
 * Leaf is a type of Node that defines action to be taken
 * While previous structures define HOW BT is to be executed,
 * Leaf defines HOW or WHAT to be executed, depending on what it describes:
 * Condition or Action.
 * There's little difference between them aside from time and purpose of execution.
 *
 * Leaf has NO children.
 */

/*
 * Execute of Leaf for one turn/step
 *
 * entity  : entity that performs action
 * context : pointer to Leaf Context
 *
 * ret     : status of execution for this step/turn
 */
typedef enum bt_node_status
    (*leaf_function_t)(entity_t* entity, void* context);
/*
 * Initialization of Leaf Context. Called on NPC creation
 *
 * entity    : entity that is to perform action
 * context   : pointer to Leaf Context
 * init_data : pointer to data used for initialization
 *
 * ret       : nothing
 */
typedef void
    (*leaf_init_function_t)(entity_t* entity, void* context, void* init_data);

struct bt_leaf {
    union {
        size_t context_size; // size of context leaf needs (in bytes)
                             //   set in code
        size_t offset;       // context offset (in bytes)
                             //   set in BT Initialization
    } u;

    leaf_function_t function;
    leaf_init_function_t init;
};

struct bt_node {

    #ifdef _DEBUG
        char* name;
    #endif

    enum {
        _BT_COMPOSITE,
        _BT_DECORATOR,
        _BT_LEAF
    } type;

    union {
        struct bt_composite composite;
        struct bt_decorator decorator;
        struct bt_leaf leaf;
    } u;

    bt_node_t* parent;
};

/*
 *  Initialize all behaviour trees
 *  MUST be called before any of them is used
 */
void s_bt_init();

/*
 * Execute BT (for current turn/step)
 * If BT is finished, starts again (from the root)
 */
int bt_execute(entity_t*);

#endif /* _BT_H_ */
