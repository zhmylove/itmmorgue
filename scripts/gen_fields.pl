#!/usr/bin/perl
# Made by kk

use strict;
use warnings;

my @f = ();

s/\n//, push @f, [split(//, $_)] while <STDIN>;
my @nf = ();

for my $i (0..@f-1) {
  for my $j (0..@{$f[$i]}-1) {
      $nf[$i][$j] = 0;
      for my $k ($i-1..$i+1) {
        $nf[$i][$j] += ($f[$k][$_] // '.') eq '"' for $j-1..$j+1;
      }
      $nf[$i][$j] -= $f[$i][$j] eq '"';
  }
}

my $finalize = 0;
for (@ARGV) {
  $finalize = 1 if /^-f$/;
}

for my $i (0..@f-1) {
  for my $j (0..@{$f[$i]}-1) {
    if ($finalize) {
      $f[$i][$j] = '.' if $f[$i][$j] =~ /[".]/ && $nf[$i][$j] <= 2;
    } else {
      $f[$i][$j] = $nf[$i][$j] >= 4 ? '"' : '.' if $f[$i][$j] =~ /[".]/;
    }
  }
}

$\ = "\n";
print join '', @$_ for @f;

