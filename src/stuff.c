// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "client.h"
#include "stuff.h"

chtype S[S_SIZE];

void stuff_init() {
    S[S_NONE            ] = ' ' | color2attr(D_BLACK)           ;
    S[S_FLOOR           ] = '.' | color2attr(L_BLACK)           ;
    S[S_WALL            ] = '#' | color2attr(D_WHITE)           ;
    S[S_DOOR            ] = '+' | color2attr(D_YELLOW)          ;
    S[S_DTREE           ] = '^' | color2attr(D_GREEN)           ;
    S[S_LTREE           ] = '^' | color2attr(L_GREEN)           ;
    S[S_DGRASS          ] = '.' | color2attr(D_GREEN)           ;
    S[S_LGRASS          ] = '.' | color2attr(L_GREEN)           ;
    S[S_PLAYER          ] = '@' | color2attr(L_YELLOW)          ;
    S[S_DOWNSTAIRS      ] = '>' | color2attr(L_WHITE)           ;
    S[S_UPSTAIRS        ] = '<' | color2attr(L_WHITE)           ;

    S[S_TRAP            ] = '^'                                 ;
    S[S_FOOD            ] = ':'                                 ;

    S[S_GOLD            ] = '$' | color2attr(L_YELLOW)          ;
    S[S_SCROLL          ] = '?' | color2attr(L_WHITE)           ;

    S[S_RING            ] = '='                                 ;
    S[S_WAND            ] = '/'                                 ;
}
