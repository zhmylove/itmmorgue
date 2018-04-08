// vim: et sw=4 ts=4 :

#ifndef _BT_MACRO_H_
#define _BT_MACRO_H_

#define ROOT(c) {.context_size=0, .child=(c)}

#define COMPOSITE(t, ...)                            \
&(bt_node_t) {                                       \
        .type = BT_COMPOSITE,                        \
        .u.composite = {                             \
            .type=(t),                               \
            .children = (bt_node_t*[]) {__VA_ARGS__} \
        }                                            \
    }                                                

#define DECORATOR(t, c)   \
&(bt_node_t) {            \
    .type = BT_DECORATOR, \
    .u.decorator = {      \
        .type=(t),        \
        .child=(c)          \
    }                     \
}                         

#define LEAF(f)              \
&(bt_node_t) {               \
    .type=BT_LEAF,           \
    .u.leaf = {              \
        .u.context_size = 0, \
        .function=(f)        \
    }                        \
}                            

#define SEQUENCE(...)    COMPOSITE(BT_SEQUENCE, __VA_ARGS__)
#define SELECTOR(...)    COMPOSITE(BT_SELECTOR, __VA_ARGS__)

#define NOT(c)           DECORATOR(BT_NOT, (c))
#define UNTIL_FAILURE(c) DECORATOR(BT_UNTIL_FAILURE, (c))


#endif /* _BT_MACRO_H_ */