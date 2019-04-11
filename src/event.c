// vim: sw=4 ts=4 et :
#include "itmmorgue.h"

// Thread object for event_loop
pthread_t ev_thread;

// File-only defines for event queue access
#define P_EV_QUEUE (players[player_id]->player_context->ev_queue)
#define P_EV_LOCK do {                                              \
    int rc;                                                         \
    if (0 != (rc = pthread_mutex_lock(&P_EV_QUEUE.event_mutex))) {  \
        if (EDEADLK == rc) {                                        \
            panic("[S] Event queue: deadlock!");                    \
        }                                                           \
        panic("[S] Event queue: failure during mutex locking");     \
    }                                                               \
} while(0)
#define P_EV_UNLOCK pthread_mutex_unlock(&P_EV_QUEUE.event_mutex)

// Add an event for specified player
void event_player_add(size_t player_id, event_t event, player_move_t *move) {
    P_EV_LOCK;

    if (P_EV_QUEUE.event_args) {
        free(P_EV_QUEUE.event_args);
    }
    P_EV_QUEUE.event = event;
    P_EV_QUEUE.event_args = (void *)move;

    P_EV_UNLOCK;
}

// Check if all players have made their events
int ev_players_ready() {
    for (size_t player_id = 0; player_id < players_len; player_id++) {
        P_EV_LOCK;
        if (P_EV_QUEUE.event == EV_NONE) {
            P_EV_UNLOCK;
            return 0;
        }
        P_EV_UNLOCK;
    }

    return 1;
}

// Get number of ready players
int ev_count_players() {
    int rc = 0;

    for (size_t player_id = 0; player_id < players_len; player_id++) {
        P_EV_LOCK;
        if (P_EV_QUEUE.event != EV_NONE) {
            rc++;
        }
        P_EV_UNLOCK;
    }

    return rc;
}

/* 
 * Current event loop idea
 *
 * 1. Collect all players events
 * 2. Collect all NPC events
 * 3. Collect all World info (?)
 * 4. Apply player events
 * 5. Calculate new NPC actions and apply them
 * 6. Calculate new game state
 * 7. Send new state to the players
 */

// Main event loop
static inline void event_loop() {
    // Do nothing before game start
    if (! start && usleep(100000) == 0) {
        return;
    }

    // 1. Collect all players events

    // Wait for the first player event
    for (; ev_count_players() == 0; usleep(10000));

    // Wait for the others player event
    for (uint32_t uticks = 0; ! ev_players_ready() && uticks < EV_TURN;
            uticks += EV_STEP) {
        usleep(EV_STEP);
    }

    // Skip this turn if there are no any players events
    if (! ev_count_players()) {
        return;
    }

    // 4. Apply player events
    for (size_t player_id = 0; player_id < players_len; player_id++) {
        P_EV_LOCK;
        switch(P_EV_QUEUE.event) {
            case EV_NONE:
                break;
            case EV_MOVE:
                player_move((player_move_t *)P_EV_QUEUE.event_args);
                break;
            default:
                panic("[S] Illegal player event!");
        }

        // Cleanup the event
        P_EV_QUEUE.event = EV_NONE;
        if (P_EV_QUEUE.event_args) {
            free(P_EV_QUEUE.event_args);
            P_EV_QUEUE.event_args = NULL;
        }
        P_EV_UNLOCK;
    }

    // 5. Calculate new NPC actions and apply them
    for (size_t id = 1; id < entities_len; id++) {
        bt_execute(entities[id]);
    }

    // 7. Send new state to the players
    for (size_t id = 0; id < players_len; id++) {
        s_send_entities_full(players[id]);
    }
}

void* event_thread(void *args) {
    for((void)args;;event_loop());
}

// Create thread for event_loop
void event_init() {
    if (pthread_create(&ev_thread, NULL, &event_thread, NULL) != 0) {
        panic("Error creating even_loop thread!");
    }
}

#undef P_EV_QUEUE
#undef P_EV_LOCK
#undef P_EV_UNLOCK
