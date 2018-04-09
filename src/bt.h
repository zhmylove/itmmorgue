// vim: et sw=4 ts=4 :

#ifndef _BT_H_
#define _BT_H_

#include "entity_decl.h"

/* BEHAVIOUR TREE */

enum bt_node_status {
    BT_SUCCESS=0,
    BT_FAILURE,
    BT_RUNNING
};

typedef struct bt_root bt_root_t;
typedef struct bt_node bt_node_t;
typedef enum bt_node_status (*leaf_function_t)(entity_t* entity, void* context);

struct bt_composite;
struct bt_decorator;
struct bt_leaf;

struct bt_root {
    size_t context_size;
    bt_node_t* child;
};

struct bt_composite {
    enum {
        BT_SEQUENCE,       // ||
        BT_SELECTOR        // && 
    } type;

    bt_node_t** children;
};

struct bt_decorator {

    enum {
        BT_NOT,            // Inversion of bt_node_status ( SUCCESS <-> FAILURE )
        BT_UNTIL_FAILURE,  // Run child until FAILURE received

        BT_EVENT_HANDLER   // Special Decorator for Event Handlers
    } type;

    bt_node_t* child;
};

struct bt_leaf {
    union {
        size_t context_size;    // size of context leaf needs (in bytes)           | set in code
        size_t offset;          // offset of leaf's context in BT's global context | set in BT Initialization
    } u;

    leaf_function_t function;
};

struct bt_node {

    enum {
        BT_COMPOSITE,
        BT_DECORATOR,
        BT_LEAF
    } type;

    union {
        struct bt_composite composite;
        struct bt_decorator decorator;
        struct bt_leaf leaf;
    } u;

    bt_node_t* parent;
};


int bt_init(bt_root_t*);
int bt_execute(entity_t*);

#endif /* _BT_H_ */