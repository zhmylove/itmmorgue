// vim: et sw=4 ts=4 :

#ifndef _BT_H_
#define _BT_H_

/* BEHAVIOUR TREE */

enum bt_node_status {
    BT_SUCCESS=0,
    BT_FAILURE,
    BT_RUNNING
};

typedef struct bt_node bt_node_t;
typedef enum bt_node_status (*leaf_function_t)(entity_t* entity, void* context);

struct bt_composite;
struct bt_decorator;
struct bt_leaf;

struct bt_root {
    size_t bt_context_size;
    bt_node_t* child;
};

struct bt_node {

    enum {
        COMPOSITE,
        DECORATOR,
        LEAF
    } type;

    union {
        struct bt_composite composite;
        struct bt_decorator decorator;
        struct bt_leaf leaf;
    } u;

    bt_node_t* parent;
};

struct bt_composite {
    enum {
        SEQUENCE,       // ||
        SELECTOR        // && 
    } type;

    bt_node_t* children[];
};

struct bt_decorator {

    enum {
        NOT,
        UNTIL_FAILURE,

        EVENT_HANDLER
    } type;

    bt_node_t* child;
};

struct bt_leaf {
    union {
        size_t context_size;    // set in code
        size_t offset;          // set in BT Initialization
    } u;

    leaf_function_t function;
}

#endif /* _BT_H_ */