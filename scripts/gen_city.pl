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

# Get random angle for the city being built
my $city_angle = int rand 4;

# Create a new level
#TODO generate random city size
gen->recreate_level_unsafe($city_angle % 2 ? (40, 20) : (20, 40));

# Generate city ground
gen->generate_blurred_area(1, ',', 0.45);

# Use ',' as free space too
gen->free_regex('[.,"^]');

#TODO Generate this building dynamically
my $building;
$building = [ map { [ split // ] } (
      "  1  ",
      "##+##",
      "#___#",
      "#___#",
      "#####",
   )
];

# Overlay the building with rotation
gen->overlay_somehow($building);

#TODO Make a loop to build multiple buildings. use eval or die

# Switch back to the main level
gen->level(0);

# Reset free space regex
gen->free_regex();

# Overlay generated city over it
gen->overlay_somehow(gen->get_level_ref(1), {rotate => [$city_angle]});

# Print the level
gen->print_level(0);
