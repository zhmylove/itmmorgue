#!/usr/bin/perl
# made by: KorG

package gen;

use strict;
use v5.18;
use warnings;
no warnings 'experimental';
use utf8;
binmode STDOUT, ':utf8';

my (@WORLD, @SIZE);
my $level = 0;

# Get or set the current level
sub level { $level = $_[1] // $level }

# (internal) Return a reference to the whole world
sub _get_world_ref { \@WORLD }

# Return a reference to the level
# opt. arg: level number
sub get_level_ref {
   my $curr = $_[1] // $level;

   $WORLD[$curr] // ($WORLD[$curr] = [])
}

# Print the level to stdout
# opt. arg: level number
sub print_level {
   my $curr = $_[1] // $level;

   do { print @$_, "\n" } for @{$WORLD[$level]};
}

# Slurp the level from stdin
# opt. arg: level number
sub read_level {
   my $curr = $_[1] // $level;

   my ($T, $H, $W) = get_level_ref();

   $/="";
   @{$T->[$H++]} = split // for split /\n/, <>;
   $W = @{$T->[0]};

   $SIZE[$curr] = {
      height => $H,
      width  => $W,
   };

   $T;
}

# Just return size[2] of the level
# opt. arg: level number
sub get_size {
   my $curr = $_[1] // $level;

   ($SIZE[$curr]->{height}, $SIZE[$curr]->{width});
}

# Recalculate the size of the level and return size[2]
# opt. arg: level number
sub update_size {
   my $curr = $_[1] // $level;

   $SIZE[$curr]->{height} = @{$WORLD[$curr]};
   $SIZE[$curr]->{width}  = @{$WORLD[$curr][0]};

   ($SIZE[$curr]->{height}, $SIZE[$curr]->{width});
}

# Overlay arg3 array over current level at arg1 x arg2 position.
# arg1: X coordinate of left upper corner
# arg2: Y
# arg3: building being built (array ref)
sub overlay_unsafe {
   my ($self, $y, $x, $building) = @_;
   my $h = @$building;
   my $T = get_level_ref();

   while ($h > 0) {
      my $ty = $y;
      $T->[$x][$ty++] = $_ for @{$building->[@$building - $h--]};
      $x++;
   }
}

# Check if area is free
# arg1: area X position
# arg2: area Y position
# arg3: area height
# arg4: area width
sub check_area_is_free {
   my ($self, $x, $y, $h, $w) = @_;
   die "Wrong number of arguments " unless @_ == 5;

   my $T = get_level_ref();

   while ($h-- > 0) {
      return 0 if grep {!/[.^]/} @{$T->[$x++]}[$y..$y+$w-1];
   }

   return 1;
}

# Get free area on the current level
# arg1: area height
# arg2: area width
sub get_free_area {
   my ($self, $h, $w) = @_;

   die "Invalid height or width" unless defined $h && defined $w;

   my ($H, $W) = get_size();

   die "Level ($W.$H) is too small for area ($w.$h)" if $H < $h || $W < $w;

   # random X and Y on the level
   my ($rx, $ry);

   #TODO correct the value
   my $ttl = 1e3;

   do {
      ($rx, $ry) = (int rand($H - 1 - $h), int rand($W - 1 - $w));
   } while ($ttl-- > 0 && not check_area_is_free(undef, $rx, $ry, $h, $w));

   die "Unable to get free area" unless $ttl > 0;

   ($ry, $rx);
}

1;
