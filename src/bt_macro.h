// vim: et sw=4 ts=4 :

#ifndef _BT_MACRO_H_
#define _BT_MACRO_H_

#define BT_ROOT(c) {.context_size=0, .child=(c)}

#ifdef _DEBUG
#define BT_SETNAME(n) .name = (n),
#else
#define BT_SETNAME(n)
#endif

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

// Following macros are the same as the ones above
// Except absence of type parameter
#define BT_SEQUENCE(name, ...)    BT_COMPOSITE(name, _BT_SEQUENCE, __VA_ARGS__)
#define BT_SELECTOR(name, ...)    BT_COMPOSITE(name, _BT_SELECTOR, __VA_ARGS__)

#define BT_NOT(name, c)              BT_DECORATOR(name, _BT_NOT, (c))
#define BT_UNTIL_FAILURE(name, c)    BT_DECORATOR(name, _BT_UNTIL_FAILURE, (c))
#define BT_SUCCEEDER(name, c)        BT_DECORATOR(name, _BT_SUCCEEDER, (c))

#define BT_ACTION(name, f)      BT_LEAF(name, f)
#define BT_CONDITION(name, f)   BT_LEAF(name, f)

#endif /* _BT_MACRO_H_ */