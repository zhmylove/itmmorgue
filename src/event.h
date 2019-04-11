// vim: sw=4 ts=4 et :
#ifndef EVENT_H
#define EVENT_H

#include "itmmorgue.h"

// Player's turn time in microseconds
#define EV_TURN 4000000
// Player's events check period in microseconds
#define EV_STEP 10000

// So shity C language
struct player_move;

typedef enum event {
    EV_NONE,        // Do nothing, event not defined (e.g. player is not ready)
    EV_WAIT,        // Do nothing, event defined (aka numpad 5, '.', etc.)
    EV_MOVE,        // Move somebody
    EV_SIZE
} event_t;

typedef struct event_queue_1 {
    event_t event;  // Only one event
    void * event_args;
    pthread_mutex_t event_mutex;
} event_queue_t;

void event_player_add(size_t player_id, event_t event,
        struct player_move *move);
void event_init();

#endif /* EVENT_H */
