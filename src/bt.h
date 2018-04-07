/* BEHAVIOUR TREE */

enum node_status {
    SUCCESS=0,
    FAILURE,
    RUNNING
};

typedef struct node btnode_t;
typedef enum node_status (*leaf_function_t)(entity_t* entity, void* context);

struct composite;
struct decorator;
struct leaf;

struct node {

    enum {
        ROOT,
        COMPOSITE,
        DECORATOR,
        LEAF
    } type;

    union {
        struct composite composite;
        struct decorator decorator;
        struct leaf leaf;
    } u;

    btnode_t* parent;
};

struct composite {
    enum {
        SEQUENCE,       // ||
        SELECTOR        // && 
    } type;

    btnode_t* children[];
};

struct decorator {

    enum {
        NOT,
        UNTIL_FAILURE,

        EVENT_HANDLER
    } type;

};

struct leaf {
    union {
        size_t context_size;    // set in code
        size_t offset;          // set in BT Initialization
    } u;

    leaf_function_t function;
}


