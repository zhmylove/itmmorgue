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

# Read zero (main) level into memory from STDIN
gen->read_level();

# Switch to the first level
gen->level(1);

# Get a reference to the first level
my $CITY = gen->get_level_ref();

# Create a new level
gen->recreate_level_unsafe(15, 10, ',');

#TODO Generate interesting city ground

# Use ',' as free space too
gen->free_regex('[.,^]');

#TODO Generate this building dynamically
my $building;
$building = [ map { [ split // ] } (
      "##+##",
      "#...#",
      "#...#",
      "#####",
   )
];

# Rotate the building somehow
gen->array_rotate($building);

# Overlay the building
gen->overlay_anywhere($building);

#TODO Make a loop to build multiple buildings. use eval or die

# Switch back to the main level
gen->level(0);

# Reset free space regex
gen->free_regex();

# Overlay generated city over it
gen->overlay_anywhere($CITY);

# Print the level
gen->print_level(0);
