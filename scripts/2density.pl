#!/usr/bin/perl
# made by: KorG

use strict;
use warnings;
use v5.18;

use GD::Simple;

my $range = 50;
my $H = 768;
my $W = 768;

my ($levels, $step_min, $step_max) = (21, 15, 55);

# T -- result
# C -- current
# N -- next
# D -- deltas
# S -- steps
my (@T, @C, @N, @D, @S);
my ($line, $abs_max, $abs_min, $steps) = (0, -~0, ~0);

srand;

sub get_line(\@) {
   my $out = $_[0];
   @$out = ();

   my $first = $range * rand;
   my $curr = $first;

   do {

      my $sign = rand >= 0.5 ? -1 : 1;

      my $count = int ($step_min + $step_max * rand);

      my $delta = $range * rand; #TODO lower boundary of diff
      my $step = ($delta - $curr) / $count;

      do {
         push @$out, $curr;
         $curr += $step;
      } while ($count-- > 0 && @$out < $W);

   } while (@$out < $W);
}

sub print_line(@) {
   printf "%02d ", int for @_;
   print "\n";
}

# first line
get_line @C;
push @{$T[$line++]}, @C;

do {
   get_line @N;

   my $max = -~0;

   for (0..$W-1) {
      $D[$_] = $N[$_] - $C[$_];
      $max = $D[$_] if $D[$_] > $max;
   }
   my $steps = int abs ++$max;
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
# print_line @{$T[$_]} for (0..$H-1);

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
