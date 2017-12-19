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

1;
