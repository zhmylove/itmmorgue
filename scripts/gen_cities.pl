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
use build;
use house;

# Read zero (main) level into memory from STDIN
gen->read_level();

# Switch to the first level
gen->level(1);

# Get random angle for the city being built
my $city_angle = int rand 4;

# Create a new level for city
my $city_factor = 100;
my $size = 1.5 * $city_factor + rand($city_factor);
my ($city_h, $city_w) = (0.4 * $size, 0.6 * $size);
gen->recreate_level_unsafe(
   $city_angle % 2 ? ($city_w, $city_h) : ($city_h, $city_w)
);

# Generate city ground
gen->generate_blurred_area(1, ',', 0.45);

# Use ',' as free space too
gen->free_regex('[.,"^]');

do {
   my $building;

   $building = rand(2) >= 1 ?  build::get_building(rand(8)+5, rand(8)+5) :
      house->build();

   # Overlay the building with rotation
   gen->overlay_somehow($building);
} for (1..6);

#TODO Make a loop to build multiple buildings. use eval or die

# Switch back to the main level
gen->level(0);

# Reset free space regex
gen->free_regex();

# Overlay generated city over it
gen->overlay_somehow(gen->get_level_ref(1), {rotate => [$city_angle]});

# Print the level
gen->print_level(0);
