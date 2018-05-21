// vim: et sw=4 ts=4 :

/*
 * Entity - a structure that defines an object with any distinct behaviour,
 * be it player, NPC, or inanimate object
 */

#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "entity_decl.h"
#include "bt.h"

#include "itmmorgue.h"
#include "connection.h"
#include "event.h"
#include "stuff.h"
#include "npc.h"

#include <stdint.h>

#define MAX_ENTITIES (2 * 1048576)
extern entity_t* entities[];
extern size_t entities_len;

/*
 * Base definition of entity for any type of it: player, NPC, etc.
 * Contains data common to any type of entity.
 */
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

/*
 * Player-specific data
 */
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

/*
 * Data specific to creature, that is, NPCs, inanimate objects, etc.
 */
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

/*
 * Data specific to inanimate objects
 */

// Defined for every type of object therefore left as a helpful comment
// struct object_context {};

/*
 * Add entity to global scope. MUST be called in the end of entity creation
 *
 * entity : pointer to entity that is to be added
 *
 * ret    : entity ID
*/
size_t entity_add(entity_t* entity);

/*
 * Create entity of specific type
 * TODO: currently a stub with square_move action
 */
size_t entity_create(enum stuff type, size_t y, size_t x);


/*
 * Network part
 */

// c2s mbuf structure
typedef struct {
    size_t ecount;  // number of entities
    size_t pcount;  // number of players
    int32_t self;   // index of player (-1 otherwise)
    entity_t entities[];
} entities_mbuf_t;

/*
 * Send entities to the player.
 * Unsafe: performance in a loss of reliability
 *
 * player : player send entities to
 * ecount : number of entities to send
 * pcount : number of players to send
 * ids    : array of entities[] id to transmit, terminated with 0.
 *
 * ret    : nothing
 */
void s_send_entities_unsafe(entity_t* player, size_t ecount, size_t pcount,
        size_t* ids);

/*
 * Send all entities to the player.
 *
 * player : player send entities to
 *
 * ret    : nothing
 */
void s_send_entities_full(entity_t* player);
void c_receive_entities(entities_mbuf_t* mbuf);

#endif /* _ENTITY_H_ */
