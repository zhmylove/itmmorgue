#ifndef DEF_CONF_H
#define DEF_CONF_H

#include "config.h"

extern struct t_conf_default {
    char *key;
    conf_t value;
} t_conf_default[];

extern size_t t_conf_default_size;

#endif /* DEF_CONF_H */
