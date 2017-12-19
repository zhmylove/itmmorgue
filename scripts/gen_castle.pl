#!/usr/bin/perl
# made by: KorG

use strict;
use v5.18;
use warnings;
no warnings 'experimental';
use utf8;
binmode STDOUT, ':utf8';
use lib '.';

use gen;

# Read a level into memory from STDIN
gen->read_level();

my @CASTLE = map { [ split // ] } (
   "###########" ,
   "#_________#" ,
   "#__#___#__#" ,
   "#__#___#__#" ,
   "#__#___#__#" ,
   "#__#_C_#__#" ,
   "#__#___#__#" ,
   "#__#####__#" ,
   "#_________#" ,
   "#####_#####" ,
   "    #_#    " ,
   "^ ^ #_# ^ ^" ,
   " ^  #_#  ^ " ,
   "^ ^     ^ ^" ,
);

# Put some buildings on the level
gen->overlay_anywhere(\@CASTLE, 2, ',');
gen->overlay_anywhere(\@CASTLE);

# Print the surface
gen->print_level();

#TODO Generate @CASTLE dynamically
