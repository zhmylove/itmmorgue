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

srand;
my $T = gen->read_level();
my ($H, $W) = gen->get_size();

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
   [ split //, "....#_#...." ],
   [ split //, "^.^.#_#.^.^" ],
   [ split //, ".^..#_#..^." ],
   [ split //, "^.^.....^.^" ],
);

# Put the building on the level
gen->overlay_unsafe(
   gen->get_free_area(scalar @CASTLE, scalar @{$CASTLE[0]}, 1, ','),
   \@CASTLE);

gen->overlay_unsafe(
   gen->get_free_area(scalar @CASTLE, scalar @{$CASTLE[0]}),
   \@CASTLE);

# Print the surface
gen->print_level();

#TODO Generate @CASTLE dynamically
