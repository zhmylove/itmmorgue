#!/usr/bin/perl
# made by: KorG

use strict;
use v5.18;
use warnings;
no warnings 'experimental';
use utf8;
binmode STDOUT, ':utf8';
use lib '.';

use gen;

# Read a level into memory from STDIN
gen->read_level();

my @CASTLE = (
   [ split //, "###########" ],
   [ split //, "#_________#" ],
   [ split //, "#__#___#__#" ],
   [ split //, "#__#___#__#" ],
   [ split //, "#__#___#__#" ],
   [ split //, "#__#_C_#__#" ],
   [ split //, "#__#___#__#" ],
   [ split //, "#__#####__#" ],
   [ split //, "#_________#" ],
   [ split //, "#####_#####" ],
   [ split //, "    #_#    " ],
   [ split //, "^ ^ #_# ^ ^" ],
   [ split //, " ^  #_#  ^ " ],
   [ split //, "^ ^     ^ ^" ],
);

# Put some buildings on the level
gen->overlay_anywhere(\@CASTLE, 2, 'x');
gen->overlay_anywhere(\@CASTLE);

# Print the surface
gen->print_level();

#TODO Generate @CASTLE dynamically
