// vim: et sw=4 ts=4 :

#include "npc.h"

#include "bt_macro.h"

bt_root_t guard_behaviour =
BT_ROOT(
    BT_SEQUENCE( Test Sequence,
        BT_UNTIL_FAILURE( Move around square,
            BT_ACTION( Move,
                square_move
            )
        )
    )
);
