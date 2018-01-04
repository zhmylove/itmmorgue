// vim: sw=4 ts=4 et :
#include "itmmorgue.h"

// Thread object for event_loop
pthread_t ev_thread;

// Add an event for specified player
void event_player_add(size_t player_id, event_t event, player_move_t *move) {
    if (players[player_id].event_args) {
        free(players[player_id].event_args);
    }
    players[player_id].event = event;
    players[player_id].event_args = (void *)move;
}

// Check if all players have made their events
int ev_players_ready() {
    for (size_t i = 0; i < players_len; i++) {
        if (players[i].event == EV_NONE) {
            return 0;
        }
    }

    return 1;
}

// Get number of ready players
int ev_count_players() {
    int rc = 0;

    for (size_t i = 0; i < players_len; i++) {
        if (players[i].event != EV_NONE) {
            rc++;
        }
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
    if (! start && usleep(10000) == 0) {
        return;
    }

    // 1. Collect all players events
    for (uint32_t uticks = 0; ! ev_players_ready() && uticks < EV_TURN;
            uticks += EV_STEP) {
        usleep(EV_STEP);
    }

    // Skip this turn if there are no any players events
    if (! ev_count_players()) {
        return;
    }

    // 4. Apply player events
    for (size_t id = 0; id < players_len; id++) {
        switch(players[id].event) {
            case EV_NONE:
                break;
            case EV_MOVE:
                player_move((player_move_t *)players[id].event_args);
                break;
            default:
                panic("[S] Illegal player event!");
        }
        // Cleanup event
        players[id].event = EV_NONE;
        if (players[id].event_args) {
            free(players[id].event_args);
        }
    }

    // 7. Send new state to the players
    for (size_t id = 0; id < players_len; id++) {
        s_send_players_full(players + id);
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
