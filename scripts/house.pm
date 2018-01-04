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
my $TYPE = undef;

# Replace all undefs with spaces and fix the whole width
sub _normalize_house {
   # Remove all undefs
   for (@HOUSE) {
      $_ = [] unless defined $_;
   }

   # Lookup for max and min defined Y
   my ($max_y, $min_y) = (0, ~0);
   for my $x (0..@HOUSE-1) {
      for my $y (0..@{$HOUSE[$x]}-1) {
         $min_y = $y if $y < $min_y && defined $HOUSE[$x][$y];
         $max_y = $y if $y > $max_y && defined $HOUSE[$x][$y];
      }
   }

   # Move the array
   for my $x (0..@HOUSE-1) {
      shift @{$HOUSE[$x]} for (0..$min_y-1);
   }

   # Fill the undefs with S_NONE
   for my $i (0..@HOUSE-1) {
      for (my $j = 0; $j <= $max_y - $min_y; $j++) {
         $HOUSE[$i][$j] = ' ' unless defined $HOUSE[$i][$j];
      }
   }
}

# Print the hosue to stdout
sub stdout {
   print @$_, "\n" for @HOUSE;
}

# (internal, unsafe) Fill specified area with chamber
sub _chamber {
   die "Wrong number of arguments " unless @_ == 4;
   my ($x, $y, $h, $w) = @_;

   $H = $x + $h if $H < $x + $h;
   $W = $y + $w if $W < $y + $w;

   my @line;
   my $i;

   @line = split//, '#' x $w;
   $i = 0;
   for (@{$HOUSE[$x++]}[$y..$y+$w-1]) {
      $_ = $line[$i] unless defined $_;
      $i++;
   }
   $h--;
   while ($h-- > 1) {
      @line = ('#', (split//, '_' x ($w - 2), $w-2), '#');
      $i = 0;
      for (@{$HOUSE[$x++]}[$y..$y+$w-1]) {
         $_ = $line[$i] unless defined $_;
         $i++;
      }
   }
   @line = split//, '#' x $w;
   $i = 0;
   for (@{$HOUSE[$x++]}[$y..$y+$w-1]) {
      $_ = $line[$i] unless defined $_;
      $i++;
   }
}

# Build the building
# arg: hashref
#  - : TYPE => type of the building
#  - : ROOMS => number of rooms (not strict)
#  - : width => width (not strict)
#  - : height => height (not strict)
#  - : xfactor and yfactor => room size multiplier
#  - : xmin and ymin => min size of rooms (strict)
sub build {
   my %c = %{$_[1] // {}};
   $TYPE       = $c{TYPE}           ;
   my $ROOMS   = $c{ROOMS}   // 2   ;
   my $width   = $c{width}   // 10  ;
   my $height  = $c{height}  // 9   ;
   my $xfactor = $c{xfactor} // 2   ;
   my $yfactor = $c{yfactor} // 1.5 ;
   my $xmin    = $c{xmin}    // 4   ;
   my $ymin    = $c{ymin}    // 4   ;

   # Initialize
   @HOUSE = ();
   ($W, $H)   = (0, 0);
   ($bx, $by) = (0, 0);

   # Select entrance position and put the entrance
   $bx = 0;
   $by = 2 + int rand($width - 1);
   $HOUSE[$bx++][$by] = '1';
   $HOUSE[$bx+1][$by] = $TYPE if defined $TYPE;
   $H++;

   # grow directions: 0 -- left, 1 -- down, 2 -- right
   my $direction = 1;

   my $rooms = 0;
   do {
      # room size
      my $rwidth = $ymin + int(rand($width + 1) / $yfactor);
      my $rheight = $xmin + int(rand($height + 1) / $xfactor);

      # Place the door on the previous coordinates
      $HOUSE[$bx][$by] = '+';

      # Attach new room to the building
      my $top_y;
      my $top_x;
      if ($direction == 1) {
         $top_x = $bx;
         do {
            $top_y = $by - 1 - int rand($rwidth / 1.5);
            $top_y = 1 if $by <= 2;
         } while ($top_y < 1);

         $rwidth += 2 if int abs $top_y - $by >= $rwidth - 1;

         _chamber($top_x, $top_y, $rheight, $rwidth);
      } elsif ($direction == 2) {
         $top_y = $by;
         do {
            $top_x = $bx - 1 - int rand($rheight / 2.5);
            $top_x = 1 if $bx <= 2;
         } while ($top_x < 1);
         $rheight += 2 if int abs $top_x - $bx >= $rheight - 1;

         _chamber($top_x, $top_y, $rheight, $rwidth);
      }

      # Choose a direction
      if ($direction == 1) {
         $direction = rand(2) >= 0.9 ? 2 : 1; # TODO s/1/0/ (really ???)
      } elsif ($direction == 2) {
         $direction = rand(2) >= 0.9 ? 1 : 2;
      } else {
         $direction = rand(2) >= 0.9 ? 1 : 2;
      }

      # Move the builder to new location
      if ($direction == 1) {
         $bx = $top_x + $rheight - 1;
         $by = $top_y + int rand($rwidth) - 2;
         $by = $top_y + 1 if $by <= $top_y;
      } elsif ($direction == 2) {
         $by = $top_y + $rwidth - 1;
         $bx = $top_x + int rand($rheight) - 2;
         $bx = $top_x + 1 if $bx <= $top_x;
      }
   } while (rand($ROOMS + 1) > $rooms++);

   # Fix all necessary things
   _normalize_house();

   # Return the house
   \@HOUSE;
}

1;
