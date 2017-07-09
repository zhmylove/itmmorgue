// vim: sw=4 ts=4 et :
#ifndef SERVER_H
#define SERVER_H

#include "itmmorgue.h"
#include "config.h"
#include "protocol.h"

#define SERVER_PORT 2607
#define SERVER_BACKLOG 8

int server_started;

void server();
void server_start();

#endif /* SERVER_H */
