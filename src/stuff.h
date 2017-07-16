// vim: sw=4 ts=4 et :
#ifndef STUFF_H
#define STUFF_H

enum stuff {
    S_NONE            ,
    S_FLOOR           ,
    S_WALL            ,
    S_DOOR            ,
    S_TREE            ,
    S_GRASS           ,
    S_PLAYER          ,
    S_DOWNSTAIRS      ,
    S_UPSTAIRS        ,

    S_TRAP            ,
    S_FOOD            ,

    S_GOLD            ,
    S_SCROLL          ,
    S_BOOK            ,

    S_RING            ,
    S_WAND            ,
    S_SIZE
};

void stuff_init();

extern chtype S[S_SIZE];

#endif /* STUFF_H */
