#!/usr/bin/perl
# Made by kk

use strict;
use warnings;

use lib '.';
use build;
use gen;

gen->read_level();

gen->overlay_anywhere(build::get_building(rand(13)+6, rand(13)+6), 2, ',');
gen->overlay_anywhere(build::get_building(rand(13)+6, rand(13)+6), 2, ',');
gen->overlay_anywhere(build::get_building(rand(13)+6, rand(13)+6), 2, ',');
gen->overlay_anywhere(build::get_building(rand(13)+6, rand(13)+6), 2, ',');
gen->overlay_anywhere(build::get_building(rand(13)+6, rand(13)+6), 2, ',');

gen->print_level();
