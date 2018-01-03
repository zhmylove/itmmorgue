#!/usr/bin/perl
# made by: KorG

package house;

use strict;
use v5.18;
use warnings;
no warnings 'experimental';
use utf8;
binmode STDOUT, ':utf8';
srand;

my @HOUSE;
my ($W, $H)   = (0, 0); # Width and Height
my ($bx, $by) = (0, 0); # Builder bot's coordinates

#TODO
#s/./ /
#s/X/_/

# Replace all undefs with spaces and fix the whole width
sub normalize_house {
   for (my $i = 0; $i < $H; $i++) {
      for (my $j = 0; $j < $W; $j++) {
         my $count = 0;
         for my $updown (map { $i + $_ } (-1..1)) {
            for (-1..1) {
               $count++ if not defined $HOUSE[$updown][$j + $_];
            }
         }

         if (defined $HOUSE[$i][$j] && $HOUSE[$i][$j] eq 'X' && $count) {
            $HOUSE[$i][$j] = '#';
         }
      }
   }

   for my $i (0..@HOUSE-1) {
      for (my $j = 0; $j < $W; $j++) {
         $HOUSE[$i][$j] = '.' unless defined $HOUSE[$i][$j];
      }
   }
}

# Print the hosue to stdout
sub stdout {
   normalize_house();

   print @$_, "\n" for @HOUSE;
}

# Check if area is free
# arg1: area X position
# arg2: area Y position
# arg3: area height
# arg4: area width
sub check_area_is_free {
   my ($self, $x, $y, $h, $w) = @_;
   die "Wrong number of arguments " unless @_ == 5;

   while ($h-- > 0) {
      return 0 if grep {defined} @{$HOUSE[$x++]}[$y..$y+$w-1];
   }

   return 1;
}

# (internal, unsafe) Fill specified area with char
sub _fill_area_with_char {
   die "Wrong number of arguments " unless @_ == 5;
   my ($x, $y, $h, $w, $pchar) = @_;

   $H = $x + $h if $H < $x + $h;
   $W = $y + $w if $W < $y + $w;

   while ($h-- > 0) {
      @{$HOUSE[$x++]}[$y..$y+$w-1] = split //, $pchar x $w;
   }
}

sub move_builder {
   my $direction = $_[0];

   sub _step {
      my $rand = rand(3) - 1;
      if ($_[0] == 1) {
         if (defined $HOUSE[$bx + 1][$by + $rand] &&
            defined $HOUSE[$bx + 1][$by + 2 * $rand] &&
            $HOUSE[$bx + 1][$by + $rand] =~ /[X+]/ &&
            $HOUSE[$bx + 1][$by + 2 * $rand] =~ /[X+]/) {
            $bx++, $by += $rand;
            return 1;
         } elsif (defined $HOUSE[$bx + 1][$by + 1] &&
            $HOUSE[$bx + 1][$by + 1] =~ /[X+.]/) {
            $bx++, $by++;
            return 1;
         } else {
            return 0;
         }
      }
   }

   while (_step($direction)) {};
}

sub build {
   my $width = 18;
   my $height = 9;
   my $xfactor = 2;
   my $yfactor = 2;
   my $xmin = 5;
   my $ymin = 5;

   # Select entrance position and put the entrance
   $bx = 0;
   $by = 1 + int rand($width - 1);
   $HOUSE[$bx++][$by] = '1';
   $H++;

   my $rwidth = $xmin + rand($width + 1) / $xfactor;
   my $rheight = $ymin + rand($height + 1) / $yfactor;

   # grow directions: 0 -- left, 1 -- down, 2 -- right
   my $direction = 1;

   # Attach new room to the building
   if ($direction == 1) {
      my $top_y;
      do {
         $top_y = $by - 1 - rand($rwidth / 2.5);
         $top_y = 1 if $by <= 2;
      } while ($top_y < 1);

      if (check_area_is_free(undef, $bx, $top_y, $rheight, $rwidth)) {
         _fill_area_with_char($bx, $top_y, $rheight, $rwidth, 'X');
      }
   }

   # Place the door on the previous coordinates
   $HOUSE[$bx][$by] = '+';

   # Choose a direction
   $direction = 1;

   # Move the builder to new location
   move_builder($direction);

   $HOUSE[$bx][$by] = '+';

   # Return the house
   @HOUSE;
}

1;
