#!/usr/bin/perl
# made by: KorG

use strict;
use v5.18;
use warnings;
no warnings 'experimental';
use utf8;

use GD::Simple;

my $h = 768;     # height
my $w = 768;     # width
my $scale = 26;  # scale for gradient color (see GD.pm)
my $burden = 50; # initial radius of changes
my $bstep = 150; # burden stepping

my @T; # surface array
my @D; # dirty flag

###
#
# SYNOPSIS
#
# This code generates a surface for smooth 2D probability distribution.
# You may be interested in dividing all of the values by $scale and
# use the result as trees density.
#
# TODO we have to decide how many times call fill_a_circle()
#
###

# Initial fill of the surface
for (my $i = 0; $i < $h; $i++) {
   for (my $j = 0; $j < $w; $j++) {
      $T[$i][$j] = 50;
      $D[$i][$j] = 0;
   }
}

# Modify a circle subarea
# arg: modification power
sub fill_a_circle($) {
   print STDERR ">";
   my $strength = $_[0];

   # - get random point
   my ($rx, $ry) = (int($h * rand), int($w * rand));
   $rx = $h if $rx > $h;
   $ry = $w if $ry > $w;
   return 0 if $D[$rx][$ry] > 1;

   # - get random delta
   my $sign = (rand >= 0.5) ? -1 : 1;
   my $delta = $sign * $strength * rand;
   # - calculate radius of change
   my $radius = int abs ($T[$rx][$ry] - $delta);
   # - fill a circle
   do {
      print STDERR "+";

      my $minX = $rx - $radius;
      $minX = 0 if $minX < 0;
      my $maxX = $rx + $radius + 1;
      $maxX = $h if $maxX > $h;
      my $minY = $ry - $radius;
      $minY = 0 if $minY < 0;
      my $maxY = $ry + $radius + 1;
      $maxY = $w if $maxY > $w;

      for (my $i = $minX; $i < $maxX; $i++) {
         for (my $j = $minY; $j < $maxY; $j++) {
            #if (($i - $rx)**2 + ($j - $ry)**2 < $radius * $radius)
            {
               $T[$i][$j] += $sign;
               $D[$i][$j] ++;
            }
         }
      }
   } while ($radius-- > 0);

   return 1;
}

do {
   $burden += $bstep if fill_a_circle($burden);
} while (sub {
      for (my $i = 0; $i < $h; $i++) {
         for (my $j = 0; $j < $w; $j++) {
            return 1 unless $D[$i][$j];
         }
      }
      return 0;
   }->() > 0);

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

print STDERR "\n";
my $img = GD::Simple->new($w, $h);
$img->bgcolor("blue");
$img->clear();
# Print the surface out
for (my $i = 0; $i < $h; $i++) {
   for (my $j = 0; $j < $w; $j++) {
      $img->line($j, $i, $j, $i,
         $img->fgcolor(sprintf("gradient%d", $T[$i][$j] + 24)));
   }
}

binmode STDOUT;
print $img->png;
