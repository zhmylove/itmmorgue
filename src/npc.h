// vim: et sw=4 ts=4 :
#include "bt_actions.h"

/*
  * Steps to implement new type of NPC/Entity with Behaviour:
  * 1. Create npc_${npcname}.c (.c file)
  * 2. Include npc.h into .c file
  * 3. Add BEHAVIOUR( ${npcname}_behaviour ) to the list below
  * 4. Construct BT for ${npcname}_behaviour variable in .c file
  *    with a help of:
  *        - bt_macro.h for structs
  *        - bt_*.h for implemented sub-behaviours and actions
*/

// BT_ARRAY MUST BE DEFINED ONLY IN bt.c
#ifdef BT_ARRAY
    #define BT_ARRAY_BEGIN bt_root_t* bt_trees[] = {
    #define BEHAVIOUR(v) &(v),
    #define BT_ARRAY_END NULL };
#else
    #define BT_ARRAY_BEGIN
    #define BEHAVIOUR(v) extern bt_root_t v;
    #define BT_ARRAY_END
#endif

/*
    LIST OF BEHAVIOURS
    1 BEHAVIOUR - 1 NPC/ENTITY WITH BEHAVIOUR
*/
BT_ARRAY_BEGIN

    BEHAVIOUR( guard_behaviour )

BT_ARRAY_END

#undef BT_ARRAY_BEGIN
#undef BEHAVIOUR
#undef BT_ARRAY_END