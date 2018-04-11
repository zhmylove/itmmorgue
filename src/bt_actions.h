// vim: et sw=4 ts=4 :

#ifndef _BT_ACTIONS_H_
#define _BT_ACTIONS_H_

#include "bt.h"
#include "bt_leaf.h"

#include <stdint.h>

struct square_move_context {
    struct {
        uint16_t x;
        uint16_t y;
    } ul_corner;
    uint16_t side_length;
    enum rotation_direction direction;
    enum direction step_direction;
};

enum bt_node_status tell_current_node(entity_t* e, void* context);
enum bt_node_status tell_current_node_fail(entity_t* e, void* context);

enum bt_node_status square_move(entity_t* e, void* context);

#endif /* _BT_ACTIONS_H_ */