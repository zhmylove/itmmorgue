// vim: et sw=4 ts=4 :

#ifndef _BT_MACRO_H_
#define _BT_MACRO_H_

#define BT_ROOT(c) {.context_size=0, .child=(c)}

#ifdef _DEBUG
#define BT_SETNAME(n) .name = (n),
#else
#define BT_SETNAME(n) 
#endif

#define BT_COMPOSITE(name, t, ...)                            \
&(bt_node_t) {                                                 \
    BT_SETNAME(#name " | " #t )                               \
    .type = _BT_COMPOSITE,                                      \
    .u.composite = {                                           \
        .type=(t),                                             \
        .children = (bt_node_t*[]) {__VA_ARGS__, NULL }        \
    }                                                          \
}                                                

#define BT_DECORATOR(name, t, c)           \
&(bt_node_t) {                              \
    BT_SETNAME(#name " | " #t)             \
    .type = _BT_DECORATOR,                   \
    .u.decorator = {                        \
        .type=(t),                          \
        .child=(c)                          \
    }                                       \
}                         

#define BT_LEAF(name, f, context)         \
&(bt_node_t) {                             \
    BT_SETNAME(#name " | _BT_LEAF")        \
    .type = _BT_LEAF,                         \
    .u.leaf = {                            \
        .u.context_size = sizeof context,  \
        .function=(f)                      \
    }                                      \
}                            

#define BT_SEQUENCE(name, ...)           BT_COMPOSITE(name, _BT_SEQUENCE, __VA_ARGS__)
#define BT_SELECTOR(name, ...)           BT_COMPOSITE(name, _BT_SELECTOR, __VA_ARGS__)

#define BT_ACTION(name, f, context)      BT_LEAF(name, (f), (context))
#define BT_CONDITION(name, f, context)   BT_LEAF(name, (f), (context))

#define BT_NOT(name, c)                  BT_DECORATOR(name, _BT_NOT, (c))
#define BT_UNTIL_FAILURE(name, c)        BT_DECORATOR(name, _BT_UNTIL_FAILURE, (c))


#endif /* _BT_MACRO_H_ */