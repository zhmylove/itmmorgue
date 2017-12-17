#!/usr/bin/perl
# made by: KorG

use strict;
use warnings;
use v5.18;

use GD::Simple;

###
#
# Generate the surface. It's the initial script for terra generation process.
#
# USAGE:
# perl gen_surface.pl [-h<Height>] [-w<Width>] | ...
#
# ENVIRONMENT:
# Basically, it should be enough to leave the ENVIRONMENT as is.
# Here are available variables:
#
# - TILES=1  (default) stdout <<< tiles, as is
# - RTILES=1           stdout <<< tiles, numbers
# - PNG=1              stdout <<< binary(PNG image)
# - TXT=1              stdout <<< human readable density map
#
###

# range -- more mountains, less plains (also vertical contraction)
# Height and Width
my $range = 40;
my $H = 90;
my $W = 130;

# levels   -- something for normalization (21 : see gradiens in GD.pm)
# step_min -- keyline distance stepping (aka horizontal contraction)
# step_max
my ($levels, $step_min, $step_max) = (0.9, 15, 55);

# T -- result
# C -- current
# N -- next
# D -- deltas
# S -- steps
my (@T, @C, @N, @D, @S);
my ($line, $abs_max, $abs_min, $steps) = (0, -~0, ~0);

# Parse options
for (@ARGV) {
   $H = $1, next if /^-h(\d+)/i;
   $W = $1, next if /^-w(\d+)/i;
   die "Usage: $0 -h<Y> -w<X>\n";
}

srand;

# Generate smoooth random line
# arg: array pointer for generated line
sub get_line(\@) {
   my $out = $_[0];
   @$out = ();

   my $first = $range * rand;
   my $curr = $first;

   do {

      my $sign = rand >= 0.5 ? -1 : 1;

      my $count = int ($step_min + $step_max * rand);

      #TODO lower boundary of delta (to exclude short transformations)
      my $delta = $range * rand;
      my $step = ($delta - $curr) / $count;

      do {
         push @$out, $curr;
         $curr += $step;
      } while ($count-- > 0 && @$out < $W);

   } while (@$out < $W);
}

# Print out any line in human-readable format
sub print_line(@) {
   if ($levels == 1) {
      printf "%02.02f ", $_ for @_;
   } else {
      printf "%02d ", int for @_;
   }
   print "\n";
}

# This function (for now) divides the whole line into S_GRASS and S_TREE,
# and prints it out in parsable format.
sub print_tiles(@) {
	die "Levels must be less than 1 for print_tiles()\n" unless $levels <= 1;

   # 4 S_TREE
   # 5 S_GRASS
   my ($S_TREE, $S_GRASS) = ("^", ".");
   ($S_TREE, $S_GRASS) = (4, 5) if $ENV{RTILES};

   #TODO get 0.3 and 0.3 magic constants from the parameters
   printf((rand($_) > rand(0.3) + 0.3) ? $S_TREE : $S_GRASS) for @_;
   print "\n";
}

# first keyline
get_line @C;
push @{$T[$line++]}, @C;

do {
   get_line @N;

   my $max = -~0;

   for (0..$W-1) {
      $D[$_] = $N[$_] - $C[$_];
      $max = $D[$_] if $D[$_] > $max;
   }
   my $steps = 1 + int abs $max;
   $S[$_] = $D[$_] / $steps for (0..$W-1);

   while ($steps-- > 0 && $line < $H) {
      for (0..$W-1) {
         $T[$line][$_] = ($C[$_] += $S[$_]);
         $abs_max = $C[$_] if $abs_max < $C[$_];
         $abs_min = $C[$_] if $abs_min > $C[$_];
      }
      $line++;
   }
} while ($line < $H);

for (my $i = 0; $i < $H; $i++) {
   for (my $j = 0; $j < $W; $j++) {
      $T[$i][$j] = $levels * ($T[$i][$j] - $abs_min) / $abs_max;
   }
}

if (1 == ($ENV{TXT} // 0)) {
   print_line @{$T[$_]} for (0..$H-1);
}

###
#
# OK. Here we have @T, which is $Hx$W array of scaled probabilities.
# Let's convert it to tiles (see stuff.h)
#
###

if ((1 == ($ENV{TILES} // 0)) ||
   (not defined $ENV{PNG} && not defined $ENV{TXT})) {
   print_tiles @{$T[$_]} for (0..$H-1);
}


if (1 == ($ENV{PNG} // 0)) {
   my $img = GD::Simple->new($W, $H);
   $img->bgcolor("blue");
   $img->clear();
   for (my $i = 0; $i < $H; $i++) {
      for (my $j = 0; $j < $W; $j++) {
         $img->line($j, $i, $j, $i,
            $img->fgcolor(sprintf("gradient%d", $T[$i][$j] + 24)));
      }
   }

   binmode STDOUT;
   print $img->png;
}

