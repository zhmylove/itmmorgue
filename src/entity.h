// vim: et sw=4 ts=4 :

#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "entity_decl.h"
#include "bt.h"

#include "itmmorgue.h"
#include "connection.h"
#include "event.h"
#include "stuff.h"

#include <stdint.h>

#define MAX_ENTITIES (2 * 1048576)
extern entity_t* entities[];
extern size_t entities_len;

struct entity {
    size_t id;

    enum {
        NONE,
        PLAYER,
        CREATURE,
        OBJECT
    } type;

    uint16_t x;
    uint16_t y;

    enum stuff stuff_type;
    enum colors color;

    struct player_context* player_context;   // Player-specific data
    void* context;
};

struct player_context {
    size_t id;

    char nickname[PLAYER_NAME_MAXLEN];
    uint8_t ready;              // ready for the game
    uint8_t connected;          // connected to the server

    // I hate that global variable, but I don't have time to fix it
    uint8_t start;              // needs data renewal
    connection_t *connection;
    event_queue_t ev_queue;

    /* player specific stats  */
};

struct creature_context {
    char name[PLAYER_NAME_MAXLEN];
    /* inventory_t */
    /* common creatures stats_t */
    /* spell_list_t */
    /* skills_t */

    bt_root_t* bt_root;
    bt_node_t* bt_current;
    void* bt_context;

    void* memory;
};

// Defined for every type of object therefore left as a helpful comment
// struct object_context {};

size_t entity_add(entity_t*);

// c2s mbuf structure
typedef struct {
    size_t ecount;  // number of entities
    size_t pcount;  // number of players
    int32_t self;   // index of player (-1 otherwise)
    entity_t entities[];
} entities_mbuf_t;

void s_send_entities_unsafe(entity_t*, size_t, size_t, size_t*);
void s_send_entities_full(entity_t* player);
void c_receive_entities(entities_mbuf_t* mbuf);

#endif /* _ENTITY_H_ */
