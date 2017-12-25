#!/usr/bin/perl
# Made by kk

use strict;
use warnings;

my @f = ();

s/\n//, push @f, [split(//, $_)] while <STDIN>;
my @nf = ();

for my $i (0..@f-1) {
  for my $j (0..@{$f[$i]}-1) {
    $nf[$i][$j] += (
      grep /"/, @{$f[$_]}[ $j-($j-1<0?0:1) .. $j+($j<@{$f[$_]}-1?1:0) ]
    ) // 0 for ($i-($i > 0 ? 1 : 0)..$i+($i < @f-1 ? 1 : 0));
    $nf[$i][$j] -= ($f[$i][$j] // '.') eq '"';
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

