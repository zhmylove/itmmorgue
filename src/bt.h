// vim: et sw=4 ts=4 :

#ifndef _BT_H_
#define _BT_H_

#include "entity_decl.h"

#include <stdlib.h> 
/* BEHAVIOUR TREE */

#define BT_MAX_NESTED_NODES 100

enum bt_node_status {
    BT_SUCCESS=0,
    BT_FAILURE,
    BT_RUNNING
};

typedef struct bt_root bt_root_t;
typedef struct bt_node bt_node_t;
typedef enum bt_node_status 
    (*leaf_function_t)(entity_t* entity, void* context);

struct bt_composite;
struct bt_decorator;
struct bt_leaf;

struct bt_root {
    size_t context_size;
    bt_node_t* child;
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
    int current_child_idx;
};

struct bt_decorator {

    enum {
        _BT_NOT,           // Inversion of bt_node_status (SUCCESS <-> FAILURE)
        _BT_UNTIL_FAILURE, // Run child until FAILURE received

        _BT_EVENT_HANDLER  // Special Decorator for Event Handlers
    } type;

    bt_node_t* child;
};

struct bt_leaf {
    union {
        size_t context_size; // size of context leaf needs (in bytes) 
                             //   set in code
        size_t offset;       // context offset (in bytes) 
                             //   set in BT Initialization
    } u;

    leaf_function_t function;
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


int bt_init(bt_root_t*);
int bt_execute(entity_t*);

#endif /* _BT_H_ */