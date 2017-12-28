// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "client.h"
#include "stuff.h"

chtype S[S_SIZE];

void stuff_init() {
    S[S_NONE        ] = conf("stuff_none").cval                           ;
    S[S_CITY        ] = conf("stuff_city").cval                           ;
    S[S_FIELD       ] = conf("stuff_field").cval                          ;
    S[S_FLOOR       ] = conf("stuff_floor").cval                          ;
    S[S_WALL        ] = conf("stuff_wall").cval                           ;
    S[S_DOOR        ] = conf("stuff_door").cval                           ;
    S[S_TREE        ] = conf("stuff_tree").cval                           ;
    S[S_GRASS       ] = conf("stuff_grass").cval                          ;
    S[S_PLAYER      ] = conf("stuff_player").cval                         ;
    S[S_DOWNSTAIRS  ] = conf("stuff_downstairs").cval                     ;
    S[S_UPSTAIRS    ] = conf("stuff_upstairs").cval                       ;

    S[S_TRAP        ] = conf("stuff_trap").cval                           ;
    S[S_FOOD        ] = conf("stuff_food").cval                           ;

    S[S_GOLD        ] = conf("stuff_gold").cval                           ;
    S[S_SCROLL      ] = conf("stuff_scroll").cval                         ;
    S[S_BOOK        ] = conf("stuff_book").cval                           ;

    S[S_RING        ] = conf("stuff_ring").cval                           ;
    S[S_WAND        ] = conf("stuff_wand").cval                           ;
    S[S_POTION      ] = conf("stuff_potion").cval                         ;
}
