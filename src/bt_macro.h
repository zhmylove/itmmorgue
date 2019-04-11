// vim: et sw=4 ts=4 :

/*
 * BT Macro - macros to ease a process of definition of Behavior Tree
 * This file contains only definitions of this macros.
 * For example, see bt_example.c
 */

#ifndef _BT_MACRO_H_
#define _BT_MACRO_H_

#define BT_ROOT(c) {.context_size=0, .child=(c)}

#ifdef _DEBUG
#define BT_SETNAME(n) .name = (n),
#else
#define BT_SETNAME(n)
#endif

/*
 * Macros to be used for BT definition
 */

/*
 * Composites
 *
 * name : name of node. Used only in DEBUG mode
 * ...  : children nodes
 */
#define BT_SEQUENCE(name, ...)    BT_COMPOSITE(name, _BT_SEQUENCE, __VA_ARGS__)
#define BT_SELECTOR(name, ...)    BT_COMPOSITE(name, _BT_SELECTOR, __VA_ARGS__)

/*
 * Decorators
 * name : name of node. Used only in DEBUG mode
 * ...  : child node
 */
#define BT_NOT(name, c)              BT_DECORATOR(name, _BT_NOT, (c))
#define BT_UNTIL_FAILURE(name, c)    BT_DECORATOR(name, _BT_UNTIL_FAILURE, (c))
#define BT_SUCCEEDER(name, c)        BT_DECORATOR(name, _BT_SUCCEEDER, (c))

/*
 * Leafs
 * name : name of node. Used only in DEBUG mode
 * f    : base name of leaf.
 *        struct f_context, f_init and f must be declared and defined somewhere
 *
 * Note: the difference between two types of leaf is only semantic
 */
#define BT_ACTION(name, f)      BT_LEAF(name, f)
#define BT_CONDITION(name, f)   BT_LEAF(name, f)


/*
 * Base Macros used for ones above.
 * It is discouraged to used this explicitly (i.e. not in the form above)
 */

/* Composite node macro
 * name : debug name of node
 * t    : type of composite
 * ...  : children
 */
#define BT_COMPOSITE(name, t, ...)                             \
&(bt_node_t) {                                                 \
    BT_SETNAME(#name " | " #t )                                \
    .type = _BT_COMPOSITE,                                     \
    .u.composite = {                                           \
        .type=(t),                                             \
        .children = (bt_node_t*[]) {__VA_ARGS__, NULL }        \
    }                                                          \
}

/* Decorator node macro
 * name : debug name of node
 * t    : type of decorator
 * c    : child
 */
#define BT_DECORATOR(name, t, c)            \
&(bt_node_t) {                              \
    BT_SETNAME(#name " | " #t)              \
    .type = _BT_DECORATOR,                  \
    .u.decorator = {                        \
        .type=(t),                          \
        .child=(c)                          \
    }                                       \
}

/* Leaf node macro
 * name : debug name of node
 * f    : pointer to function for execution
 * c    : context type (primitive/structure).
 *        Used with sizeof
 *
 * NOTE: for every leaf function f()
 * struct f_context and f_init() must be declared and defined
 */
#define BT_LEAF(name, f)                              \
&(bt_node_t) {                                        \
    BT_SETNAME(#name " | _BT_LEAF")                   \
    .type = _BT_LEAF,                                 \
    .u.leaf = {                                       \
        .u.context_size = sizeof(struct f##_context), \
        .function=(f),                                \
        .init=(f##_init)                              \
    }                                                 \
}

#endif /* _BT_MACRO_H_ */