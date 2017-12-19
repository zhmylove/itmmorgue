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
   [ split //, "#.........#" ],
   [ split //, "#..#...#..#" ],
   [ split //, "#..#...#..#" ],
   [ split //, "#..#...#..#" ],
   [ split //, "#..#.C.#..#" ],
   [ split //, "#..#...#..#" ],
   [ split //, "#..#####..#" ],
   [ split //, "#.........#" ],
   [ split //, "#####.#####" ],
   [ split //, "....#.#...." ],
   [ split //, "^.^.#.#.^.^" ],
   [ split //, ".^..#.#..^." ],
   [ split //, "^.^.....^.^" ],
);

# Get random point for the castle
my ($ry, $rx) = gen->get_free_area(scalar @CASTLE, scalar @{$CASTLE[0]});

# Put the building on the level
gen->overlay_unsafe($ry, $rx, \@CASTLE);

($ry, $rx) = gen->get_free_area(scalar @CASTLE, scalar @{$CASTLE[0]});
gen->overlay_unsafe($ry, $rx, \@CASTLE);

# Print the surface
gen->print_level();

#TODO
# * Improve $rx x $ry generation with padding to avoid overflows
# * Generate @CASTLE dynamically
