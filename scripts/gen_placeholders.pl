#!/usr/bin/perl
# made by: KorG

use strict;
use v5.18;
use warnings;
no warnings 'experimental';
use utf8;
binmode STDOUT, ':utf8';

# Read our placeholders and their characters
open my $fh, "<:utf8", "stuff" or die $!;
my (@F, %C);
while (defined($_ = <$fh>)) {
   @F = split /\s+/, $_, 5;

   next unless @F > 3;

   next unless $F[1] =~ m{\[(.)\]};
   my $char = $1;

   $C{$1} = $char if $F[3] =~ m{\[(.)\]};
}

my ($from, $to);
for (keys %C) {
   $from .= $_;
   $to   .= $C{$_};
}

my ($rfrom, $rto) = (qr"$from", qr"$to");

eval "y/$rfrom/$rto/", print while (defined($_ = <>));
