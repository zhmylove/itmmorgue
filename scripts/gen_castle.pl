#!/usr/bin/perl
# made by: KorG

use strict;
use v5.18;
use warnings;
no warnings 'experimental';
use utf8;
binmode STDOUT, ':utf8';

my @T;
my ($H, $W);

srand;

# Read all stdin and evalueate Height and Width
$/="";
@{$T[$H++]} = split // for split /\n/, <>;
$W = @{$T[0]};

my @CASTLE = (
   [ "#", "#", "#", "#", "#", "#", "#", "#", "#", "#", "#" ],
   [ "#", ".", ".", ".", ".", ".", ".", ".", ".", ".", "#" ],
   [ "#", ".", ".", "#", ".", ".", ".", "#", ".", ".", "#" ],
   [ "#", ".", ".", "#", ".", ".", ".", "#", ".", ".", "#" ],
   [ "#", ".", ".", "#", ".", ".", ".", "#", ".", ".", "#" ],
   [ "#", ".", ".", "#", ".", "C", ".", "#", ".", ".", "#" ],
   [ "#", ".", ".", "#", ".", ".", ".", "#", ".", ".", "#" ],
   [ "#", ".", ".", "#", "#", "#", "#", "#", ".", ".", "#" ],
   [ "#", ".", ".", ".", ".", ".", ".", ".", ".", ".", "#" ],
   [ "#", "#", "#", "#", "#", ".", "#", "#", "#", "#", "#" ],
   [ ".", ".", ".", ".", "#", ".", "#", ".", ".", ".", "." ],
   [ "^", ".", "^", ".", "#", ".", "#", ".", "^", ".", "^" ],
   [ ".", "^", ".", ".", "#", ".", "#", ".", ".", "^", "." ],
   [ "^", ".", "^", ".", ".", ".", ".", ".", "^", ".", "^" ],
);

# Overlay arg3 array over @T at arg1 x arg2 position.
# arg1: X coordinate of left upper corner
# arg2: Y
# arg3: building being built (array)
sub move_array_unsafe($$\@) {
   my ($y, $x, $building) = @_;
   my @building = @$building;
   my $h = @building;

   while ($h > 0) {
      my $ty = $y;
      $T[$x][$ty++] = $_ for @{$building[@building - $h--]};
      $x++;
   }
}

my ($rx, $ry) = (int rand($H - 1), int rand($W - 1));
$T[$rx][$ry] = "O"; # random point for castle (see TODO)

move_array_unsafe(15, 5, @CASTLE);

# Print the surface
print @{$T[$_]}, "\n" for (0..$H-1);

#TODO
# 1. Move the castle to $rx x $ry
# 2. Check if there is enough space for the castle, or regenerate $rx x $ry
# 3. Improve $rx x $ry generation with padding to avoid overflows
# 4. Generate @CASTLE dynamically
