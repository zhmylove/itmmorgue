#!/usr/bin/perl
# Made by kk

package build;

##############################################################
#                                                            #
#                ____________________________                #
#               /............................\    """""      #
#              /........+============+........\   """""      #
#             /.........|     ++     |.........\  """""      #
#            /..........|     ++     |..........\ """""      #
#           /...........|++++++++++++|...........\"""""      #
#          /............|     ++     |............\""""      #
#         /.............|     ++     |.............\"""      #
#        /..............+============+..............\""      #
#       /............................................\"      #
#      /..............................................\      #
#     /................................................\     #
#    ====================================================    #
#   |...............................................,....|   #
#   |.....=============..................,........,,.....|   #
#   |....+=============+............,,.,........,........|   #
#   |....|      |      |.................................|   #
#   |....|      |      |........,........................|   #
#   |....|      |      |......,......,###############....|   #
#   |....+------+------+.............,#             #....|   #
#   |....|      |      |..........,,..#             #....|   #
#   |....|      |      |...,..........#             #....|   #
#   |....|      |      |....,.........#             #....|   #
#   |....+=============+..............#             #....|   #
#   |.....=============.......,.......#  0          #....|   #
#   |..,....................,.,...... #             #....|   #
#   |....,...............,........... #  ©          #....|   #
#   |..,............,.......,........ #             #....|   #
#   |......,,.....,,................. #             #....|   #
#   |.....,.....,.................... #             #....|   #
#   +----------------------------------------------------+   #
#                                                            #
##############################################################

use strict;
use warnings FATAL => 'all';

# Places 1 near main door.
sub _decorate {
  my ($edx, $edy, $w, $h) = @_;
  my @bldg = @{$_[4]};
  my $sx = $edx == 0;
  my $sy = $edy == 0;
  my @nbldg = ();
  for my $i (0..$h-1) {
    for my $j (0..$w-1) {
      $nbldg[$i+$sy][$j+$sx] = $bldg[$i][$j];
    }
  }
  if ($edy == 0 || $edy == $h-1) { 
    $edy = $h if $edy == $h-1;
    $nbldg[$edy][$_] = ' ' for 0..$w-1;
    $nbldg[$edy == 0 ? 0 : $h][$edx] = '1';
  }
  if ($edx == 0 || $edx == $w-1) {
    $edx = $w if $edx == $w-1;
    $nbldg[$_][$edx] = ' ' for 0..$h-1;
    $nbldg[$edy][$edx == 0 ? 0 : $w] = '1';
  }

  \@nbldg;
}


# Returns new two-dimensional array with building.
#
# arg0 : width  (optional)
# arg1 : height (optional)
# ret  : building
sub get_building {
  my $w = $_[0] // 20;
  my $h = $_[1] // 20;
  $w = int $w; $h = int $h;
  die "Building width and height must be at least 5 tails"
  unless $w >= 5 && $h >= 5;

  # Adds some doors to building
  sub place_doors {
    my ($w, $h) = @_;
    my @bldg = @{$_[2]};
    for my $i (0..$h-1) {
      for my $j (0..$w-1) {
        $bldg[$i][$j] = '+' if $bldg[$i][$j] eq '#' && rand() < 0.03;
      }
    }
    for my $i (0..$h-1) {
      for my $j (0..$w-1) {
        if ($bldg[$i][$j] eq '+') {
          my $cnt = 0;
          # Doors count
          $cnt += (
            grep {$_ eq '+'}
            @{$bldg[$_]}[$j-($j-1<0?0:1)..$j+($j<$w-1?1:0)]
          ) // 0 for ($i-($i > 0 ? 1 : 0)..$i+($i < $h-1 ? 1 : 0));
          $bldg[$i][$j] = '#', next if 1 != $cnt;
          # Door angleness
          next if 2 == ($cnt =
            ($i == 0    || $bldg[$i-1][$j] eq '_') +
            ($i == $h-1 || $bldg[$i+1][$j] eq '_')
          );
          $bldg[$i][$j] = '#', next if 1 == $cnt;
          next if 2 == (
            ($j == 0    || $bldg[$i][$j-1] eq '_') +
            ($j == $w-1 || $bldg[$i][$j+1] eq '_')
          );
          $bldg[$i][$j] = '#';
        }
      }
    }
  }

  # Checks reachability of each non-wall point
  sub everything_is_reachable {
    my ($edy, $edx, $w, $h) = @_;
    my @bldg = @{$_[4]};

    return undef unless $bldg[$edy][$edx] eq '+';

    my @visited = (); # Map of visited points
    my @q       = (); # Queue for start points
    push @q, [$edy, $edx];
    $visited[$edy][$edx] = 1;
    while (0 != @q) {
      my ($y, $x) = @{shift @q};
      next if $y < 0 || $y > $h-1 || $x < 0 || $x > $w-1;

      for my $i ($y-1..$y+1) {
        next if $i < 0 || $i > $h-1;
        for my $j ($x-1..$x+1) {
          next if $j < 0 || $j > $w-1;
          # We've just checked building borders.
          # Now we have to check that we are looking onto door. It means that
          # should check reachability without diagonal tiles.
          next if (
            '+' eq $bldg[$i][$j] || '+' eq $bldg[$y][$x]
          ) && (abs($x-$j)+abs($y-$i) != 1);
          unless ($visited[$i][$j] || $bldg[$i][$j] eq '#') {
            push @q, [$i, $j];
            $visited[$i][$j] = 1;
          }
        }
      }
    }

    for my $i (0..$h-1) {
      for my $j (0..$w-1) {
        return undef unless $bldg[$i][$j] eq '#' || $visited[$i][$j];
      }
    }

    1;
  }

  my @bldg = (); # building

  # Initial floor using S_FLOOR
  for my $i (1..$h-2) {
    for my $j (1..$w-2) {
      $bldg[$i][$j] = '_';
    }
  }

  # Main walls using S_WALL
  ($bldg[$_][0], $bldg[$_][$w-1]) = ('#', '#') for 0..$h-1;
  ($bldg[0][$_], $bldg[$h-1][$_]) = ('#', '#') for 0..$w-1;

  # Rooms
  for (0..$h*0.11) {
    my $y = int rand $h;
    $bldg[$y][$_] = '#' for 0..$w-1;
  }

  for (0..$w*0.11) {
    my $x = int rand $w;
    $bldg[$_][$x] = '#' for 0..$h-1;
  }

  # Get rid of fat walls
  my ($tries, $wall_fixed) = ($w*$h, 1);
  while ($tries-- > 0 && $wall_fixed) {
    $wall_fixed = undef;
    for my $i (1..$h-2) {
      for my $j (1..$w-3) {
        next unless '#' eq $bldg[$i][1];
        # Next line is wall or previous line is wall. And this line is wall.
        if ('#' eq $bldg[$i][$j+1] &&
          ('#' eq $bldg[$i-1][$j] && '#' eq $bldg[$i-1][$j+1] ||
            ('#' eq $bldg[$i+1][$j] && '#' eq $bldg[$i+1][$j+1]))) {
          $bldg[$i][$_] = '_' for 1..$w-2;
          $wall_fixed = 1;
        }
      }
    }

    for my $i (1..$w-2) {
      for my $j (1..$h-3) {
        next unless '#' eq $bldg[1][$i];
        # Next line is wall or previous line is wall. And this line is wall.
        if ('#' eq $bldg[$j+1][$i] &&
          ('#' eq $bldg[$j][$i-1] && '#' eq $bldg[$j+1][$i-1] ||
            ('#' eq $bldg[$j][$i+1] && '#' eq $bldg[$j+1][$i+1]))) {
          $bldg[$_][$i] = '_' for 1..$h-2;
        }
      }
    }

    # Fix leaky walls
    for my $i (1..$h-2) {
      for my $j (1..$w-2) {
        if ('#' eq $bldg[$i][$j]) {
          if (1 == ('#' eq $bldg[$i-1][$j]) + ('#' eq $bldg[$i+1][$j])) {
            # Try to fix broken vertical line
            # Find closest wall on the left
            my $wpos;
            for (reverse 1..$j-1) {
              $wpos = $_ and last if '#' eq $bldg[$i][$_];
            }
            if (defined $wpos) {
              # Conduct to the left
              $bldg[$i][$_] = '#' for $wpos..$j-1;
              $wall_fixed = 1;
              next;
            }

            # Find closest wall on the right
            for ($j+1..$w-2) {
              $wpos = $_ and last if '#' eq $bldg[$i][$_];
            }
            if (defined $wpos) {
              # Conduct to the right
              $bldg[$i][$_] = '#' for $j+1..$wpos;
              $wall_fixed = 1;
              next;
            }

            # Haven't found, make full vertical line
            $wall_fixed = 1;
            $bldg[$_][$j] = '#' for 0..$h-1;
            next;
          }

          if (1 == ('#' eq $bldg[$i][$j-1]) + ('#' eq $bldg[$i][$j+1])) {
            # Try to fix broken horizontal line
            # Find closest wall on the top
            my $wpos;
            for (reverse 1..$i-1) {
              $wpos = $_ and last if '#' eq $bldg[$_][$j];
            }
            if (defined $wpos) {
              # Conduct to the top
              $bldg[$_][$j] = '#' for $wpos..$i-1;
              $wall_fixed = 1;
              next;
            }

            # Find closest wall on the bottom
            for ($i+1..$h-2) {
              $wpos = $_ and last if '#' eq $bldg[$_][$j];
            }
            if (defined $wpos) {
              # Conduct to the bottom
              $bldg[$_][$j] = '#' for $i+1..$wpos;
              $wall_fixed = 1;
              next;
            }

            # Haven't found, make full horizontal line
            $wall_fixed = 1;
            $bldg[$i][$_] = '#' for 0..$w-1;
          }
        }
      }
    }
  }

  my ($edy, $edx); # Enter Door coordinates
  if (rand() > 0.5) {
    $edy = int(rand() * ($h-3)) + 1;
    $edx = rand() > 0.5 ? 0 : $w-1;
  } else {
    $edx = int(rand() * ($w-3)) + 1;
    $edy = rand() > 0.5 ? 0 : $h-1;
  }
  $bldg[$edy][$edx] = '+';

  # Plant some doors
  place_doors $w, $h, \@bldg;
  return _decorate $edx, $edy, $w, $h, \@bldg
  if everything_is_reachable $edy, $edx, $w, $h, \@bldg;
  # Try one more time
  place_doors $w, $h, \@bldg;
  return _decorate $edx, $edy, $w, $h, \@bldg
  if everything_is_reachable $edy, $edx, $w, $h, \@bldg;

  # Sometime we'll be lucky
  get_building(@_);
}

1;
