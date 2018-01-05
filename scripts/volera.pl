#!/usr/bin/perl
# made by: KorG

use strict;
use v5.18;
use warnings;
no warnings 'experimental';
use utf8;
binmode STDOUT, ':utf8';

use lib '.';
use build;

print @$_, "\n" for @{build::get_building(rand(13)+6, rand(13)+6)};
