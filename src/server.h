// vim: sw=4 ts=4 et :
#ifndef SERVER_H
#define SERVER_H

#include "itmmorgue.h"
#include "config.h"
#include "protocol.h"
#include "connection.h"

#define SERVER_PORT 2607
#define SERVER_BACKLOG 8

int server_started;

void server();
void server_fork_start();

void* process_client(connection_t *connection);

/*
 * Global game state. 
 * 0 = new game, never played
 * 1 = game created, populating players with areas
 * 2 = normal game state
 * 3 = resurrection phase
 * 4 = resurrected by nickname, needs to be repopulated
 */
extern char start;

#endif /* SERVER_H */
