// vim: sw=4 ts=4 et :

/*
 * Example of Behaviour Tree defined with Macros from bt_macro.h
 * To be updated after any adjustment or extension to BT,
 * e.g. refactoring, architecture change,
 *      adding new composite, decorator, etc.
 * Since this BT does nothing (or prints names of Nodes in DEBUG mode),
 * it is ill-advised to use this tree as a behaviour of any actual entity
 */

#include "npc.h"

#include "bt_macro.h"

bt_root_t test_behaviour =
BT_ROOT(                                // Any behavior starts with Root
                                        // The only argument - its only child

    BT_SEQUENCE(Main sequence,          // Any node has Name as first argument
                                        // It exists only in DEBUG mode
        BT_SELECTOR(Simple Selector,    // Selector and Sequence are Composites
                                        // Composite may have 1+ children

            BT_ACTION(Will happen,      // Action is a type of Leaf
                                        // Leaf is the only type to be executed
                                        // i.e. that actually does something

                tell_current_node_fail  // Action itself (pointer to function)
            ),

            BT_ACTION(This will end selector,
                tell_current_node
            ),
            BT_ACTION(Wont happen,
                tell_current_node
            )
        ),
        BT_SELECTOR(Selector with succeeder,
            BT_SUCCEEDER(Succeeder,     // Succeeder is a type of Decorator
                                        // Decorators have only 1 child
                                        // They alter execution of their children
                                        // Succeeder always returns Success
                BT_ACTION(Failed. But Succeeder saved us,
                    tell_current_node_fail
                )
            ),
            BT_ACTION(Wont happen too,
                tell_current_node
            )
        ),
        // Execute child until Failure is received
        BT_UNTIL_FAILURE(Until failure decorator,
            BT_SEQUENCE(Sequence,
                BT_ACTION(Normal execution,
                    tell_current_node
                ),
                BT_ACTION(Ends UNTIL FAILURE,
                    tell_current_node_fail
                ),
                BT_ACTION(WONT HAPPEN III,
                    tell_current_node
                )
            )
        ),
        BT_NOT(Simple inversion,        // Decorator that inverts Success <-> Failure
            BT_ACTION(Fails and is inverted,
                tell_current_node_fail
            )
        ),
        BT_ACTION(Fail,                 // This action fails main sequence
                                        // BT is restarted from the root
            tell_current_node_fail
        ),
        BT_ACTION(Wont happen IV,
            tell_current_node
        )
    )
);