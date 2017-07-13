// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "client.h"
#include "stuff.h"

chtype S[S_SIZE];

void stuff_init() {
    S[S_NONE        ] = conf("stuff_none").cval       | color2attr(D_BLACK)   ;
    S[S_FLOOR       ] = conf("stuff_floor").cval      | color2attr(L_BLACK)   ;
    S[S_WALL        ] = conf("stuff_wall").cval       | color2attr(D_WHITE)   ;
    S[S_DOOR        ] = conf("stuff_door").cval       | color2attr(D_YELLOW)  ;
    S[S_DTREE       ] = conf("stuff_dtree").cval      | color2attr(D_GREEN)   ;
    S[S_LTREE       ] = conf("stuff_ltree").cval      | color2attr(L_GREEN)   ;
    S[S_DGRASS      ] = conf("stuff_dgrass").cval     | color2attr(D_GREEN)   ;
    S[S_LGRASS      ] = conf("stuff_lgrass").cval     | color2attr(L_GREEN)   ;
    S[S_PLAYER      ] = conf("stuff_player").cval     | color2attr(L_YELLOW)  ;
    S[S_DOWNSTAIRS  ] = conf("stuff_downstairs").cval | color2attr(L_WHITE)   ;
    S[S_UPSTAIRS    ] = conf("stuff_upstairs").cval   | color2attr(L_WHITE)   ;

    S[S_TRAP        ] = conf("stuff_trap").cval                               ;
    S[S_FOOD        ] = conf("stuff_food").cval                               ;

    S[S_GOLD        ] = conf("stuff_gold").cval       | color2attr(L_YELLOW)  ;
    S[S_SCROLL      ] = conf("stuff_scroll").cval     | color2attr(L_WHITE)   ;
    S[S_BOOK        ] = conf("stuff_book").cval       | color2attr(D_YELLOW)  ;

    S[S_RING        ] = conf("stuff_ring").cval                               ;
    S[S_WAND        ] = conf("stuff_wand").cval                               ;
}
