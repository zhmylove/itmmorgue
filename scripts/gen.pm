#!/usr/bin/perl
# made by: KorG

package gen;

use strict;
use v5.18;
use warnings;
no warnings 'experimental';
use utf8;
binmode STDOUT, ':utf8';
srand;

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
      for (@{$building->[@$building - $h--]}) {
         $ty++ and next unless defined $_ && !/[ ]/;
         $T->[$x][$ty++] = $_;
      }
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
      return 0 if grep {!/[.,^]/} @{$T->[$x++]}[$y..$y+$w-1];
   }

   return 1;
}

# (internal, unsafe) Fill specified area with char
sub _fill_area_with_char {
   die "Wrong number of arguments " unless @_ == 5;
   my ($x, $y, $h, $w, $pchar) = @_;

   my $T = get_level_ref();
   while ($h-- > 0) {
      @{$T->[$x++]}[$y..$y+$w-1] = split //, $pchar x $w;
   }
}

# Get free area on the current level
# arg1:      area height
# arg2:      area width
# opt. arg3: area padding
# opt. arg4: padding character
sub get_free_area {
   my ($self, $h, $w, $p, $pchar) = @_;

   # default padding is 0
   $p = 0 unless defined $p;

   die "Invalid height or width" unless defined $h && defined $w;

   my ($H, $W) = get_size();

   die "Level ($W.$H) is too small for area ($w.$h) with padding $p!" if
   ($H < $h + 2 * $p) || ($W < $w + 2 * $p);

   # random X and Y on the level
   my ($rx, $ry);

   #TODO correct ttl value
   my $ttl = 1e3;

   do {
      $rx = $p + int rand($H - 1 - $h - 2 * $p);
      $ry = $p + int rand($W - 1 - $w - 2 * $p);
   } while ($ttl-- > 0 && not check_area_is_free(
         undef, $rx - $p, $ry - $p, $h + 2 * $p, $w + 2 * $p
      )
   );

   die "Unable to get free area" unless $ttl > 0;

   # fill padding area if character specified
   #TODO fill only padding
   _fill_area_with_char($rx - $p, $ry - $p, $h + 2 * $p, $w + 2 * $p, $pchar)
   if defined $pchar;

   ($ry, $rx);
}

# Overlay arg1 array over current level on free space
# arg1:      2d array reference
# opt. arg2: padding
# opt. arg3: padding character
sub overlay_anywhere {
   my ($self, $array, $padding, $pchar) = @_;

   die "Not an ARRAY reference at all: $array" unless ref $array eq "ARRAY";
   die "Not an ARRAY reference: $array->[0]" unless ref $array->[0] eq "ARRAY";

   overlay_unsafe(undef, get_free_area(
         undef, scalar @$array, scalar @{$array->[0]}, $padding, $pchar
      ),
      $array
   );
}

1;
