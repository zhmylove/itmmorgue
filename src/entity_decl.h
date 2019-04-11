// vim: et sw=4 ts=4 :

/*
 * Separate header for basic definitions of entity types.
 * Used and created to get rid of circular dependency
 * (entity.h -> bt.h -> entity.h)
 */

#ifndef _ENTITY_DEF_H_
#define _ENTITY_DEF_H_

struct entity;
struct player_context;
struct creature_context;

typedef struct entity entity_t;

#endif /* _ENTITY_DEF_H_ */
