// vim: et sw=4 ts=4 :

/*
 * Common Actions for Behaviour Tree
 *
 * This file defines Common Actions for Behaviour Tree,
 * i.e. Actions that can be used by multiple entities
 * Actions function has signature void f(entity_t* entity, void* context)
 * Where context - is structure specifically defined for this action.
 *
 * Note: BT Action = BT Leaf. The only difference is semantics.
 *
 * Format of Action for this file is following:
 *  - Action Context    - structure that defines "variables" on which
 *                        actual Action depends on.
 *       Format: struct action_context {}
 *
 *  - Action Initializer - function that initializes context for action.
 *                         Called on NPC creation
 *       Format: see leaf_init_function_t in bt.h
 *
 *  - Action Function    - function that implements Action itself,
 *                         Is to be called each turn of Action is active in BT
 *       Format: see leaf_function_t in bt.h
 */

#ifndef _BT_ACTIONS_H_
#define _BT_ACTIONS_H_

#include "bt.h"
#include "bt_leaf.h"

#include <stdint.h>

#define EMPTY_LEAF_CONTEXT(f) struct f##_context {char empty;}

/*
 * Move around square
 * ul : coordinates of Upper-Left corner
 * side_length : length of sides
 *
 */
struct square_move_context {
    struct {
        uint16_t x;
        uint16_t y;
    } ul_corner;
    uint16_t side_length;
    enum rotation_direction direction;
    enum direction step_direction;
};

enum bt_node_status square_move(entity_t* e, void* context);
void square_move_init(entity_t *, void*, void*);

/*
 * Tells name of current node (in DEBUG mode) or does nothing (in normal mode)
 * These are stubs for testing
 * It's highly discouraged to use them for actual NPCs
 * _fail version differs only by returning BT_FAILURE instead of BT_SUCCESS
 */

EMPTY_LEAF_CONTEXT(tell_current_node);
EMPTY_LEAF_CONTEXT(tell_current_node_fail);

enum bt_node_status tell_current_node(entity_t* e, void* context);
enum bt_node_status tell_current_node_fail(entity_t* e, void* context);

void tell_current_node_init(entity_t *, void* a, void* b);
void tell_current_node_fail_init(entity_t *, void* a, void* b);

#undef EMPTY_LEAF_CONTEXT

#endif /* _BT_ACTIONS_H_ */
