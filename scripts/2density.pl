#!/usr/bin/perl
# made by: KorG

use strict;
use v5.18;
use warnings;
no warnings 'experimental';
use utf8;
binmode STDOUT, ':utf8';

my $h = 30;     # height
my $w = 20;     # width
my $scale = 50; # nothing

my @T;          # surface array

###
#
# SYNOPSIS
#
# This code generates a surface for smooth 2D probability distribution.
# You may be interested in dividing all of the values by $scale and
# use the result as trees density.
#
# TODO we have to solve how many times should we call fill_a_circle()
# concerning $h and $w.
#
###

# Initial fill of the surface
for (my $i = 0; $i < $h; $i++) {
   for (my $j = 0; $j < $w; $j++) {
      $T[$i][$j] = 50;
   }
}

sub fill_a_circle {
   # - get random point
   my ($rx, $ry) = (int($h * rand), int($w * rand));
   $rx = $h if $rx > $h;
   $ry = $w if $ry > $w;
   # - get random delta
   my $sign = (rand >= 0.5) ? -1 : 1;
   my $delta = $sign * 50 * rand;
   # - calculate radius of change
   my $radius = int abs ($T[$rx][$ry] - $delta);
   # - fill a circle
   do {
      for (my $i = 0; $i < $h; $i++) {
         for (my $j = 0; $j < $w; $j++) {
            if (($i - $rx)**2 + ($j - $ry)**2 < $radius * $radius) {
               $T[$i][$j] += $sign;
            }
         }
      }
   } while ($radius-- > 0);
}

fill_a_circle();
fill_a_circle();
fill_a_circle();
#TODO how many times?

# Fix negatives
for (my $i = 0; $i < $h; $i++) {
   for (my $j = 0; $j < $w; $j++) {
      $T[$i][$j] = abs $T[$i][$j];
   }
}

# Get max and min values
my ($max, $min) = (-~0, ~0);
for (my $i = 0; $i < $h; $i++) {
   for (my $j = 0; $j < $w; $j++) {
      $max = $T[$i][$j] if $max < $T[$i][$j];
      $min = $T[$i][$j] if $min > $T[$i][$j];
   }
}

# Move the surface down
for (my $i = 0; $i < $h; $i++) {
   for (my $j = 0; $j < $w; $j++) {
      $T[$i][$j] -= $min;
   }
}

# Normalize the surface
$max -= $min;
for (my $i = 0; $i < $h; $i++) {
   for (my $j = 0; $j < $w; $j++) {
      $T[$i][$j] = $scale * $T[$i][$j] / $max;
   }
}

# Print the surface out
for (my $i = 0; $i < $h; $i++) {
   for (my $j = 0; $j < $w; $j++) {
      printf "%02d ", $T[$i][$j];
   }
   print "\n";
}
