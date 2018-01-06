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

# Load stuff
our %stuff = ();
open my $stuff, '<', 'stuff';
s/S_([^ ]+) *\[(.)\]/$stuff{lc$1} = $2/e for <$stuff>;
close $stuff;


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


# Get new two-dimensional array with building.
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

  # Checks that arg is wall
  sub wallp { '#' eq $_[0]; }

  # Adds some doors to building
  sub place_doors {
    my ($w, $h) = @_;
    my @bldg = @{$_[2]};
    for my $i (0..$h-1) {
      for my $j (0..$w-1) {
        $bldg[$i][$j] = $stuff{door} if wallp($bldg[$i][$j]) && rand() < 0.03;
      }
    }
    for my $i (0..$h-1) {
      for my $j (0..$w-1) {
        if ($bldg[$i][$j] eq $stuff{door}) {
          my $cnt = 0;
          # Doors count
          $cnt += (
            grep {$_ eq $stuff{door}}
            @{$bldg[$_]}[$j-($j-1<0?0:1)..$j+($j<$w-1?1:0)]
          ) // 0 for ($i-($i > 0 ? 1 : 0)..$i+($i < $h-1 ? 1 : 0));
          $bldg[$i][$j] = $stuff{wall} if 1 != $cnt;
        }
      }
    }
  }

  # Check reachability
  sub everything_is_reachable {
    my ($edy, $edx, $w, $h) = @_;
    my @bldg = @{$_[4]};

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
            $stuff{door} eq $bldg[$i][$j] || $stuff{door} eq $bldg[$y][$x]
          ) && (abs($x-$j)+abs($y-$i) != 1);
          unless ($visited[$i][$j] || wallp($bldg[$i][$j])) {
            push @q, [$i, $j];
            $visited[$i][$j] = 1;
          }
        }
      }
    }

    for my $i (0..$h-1) {
      for my $j (0..$w-1) {
        return undef unless wallp($bldg[$i][$j]) || $visited[$i][$j];
      }
    }

    1;
  }

  my @bldg = (); # building

  # Initial floor using S_FLOOR
  for my $i (1..$h-2) {
    for my $j (1..$w-2) {
      $bldg[$i][$j] = $stuff{floor};
    }
  }

  # Main walls using S_WALL
  ($bldg[$_][0], $bldg[$_][$w-1]) = ($stuff{wall}, $stuff{wall}) for 0..$h-1;
  ($bldg[0][$_], $bldg[$h-1][$_]) = ($stuff{wall}, $stuff{wall}) for 0..$w-1;

  # Rooms
  for (0..$h*0.11) {
    my $y = int rand $h;
    $bldg[$y][$_] = $stuff{wall} for 0..$w-1;
  }

  for (0..$w*0.11) {
    my $x = int rand $w;
    $bldg[$_][$x] = $stuff{wall} for 0..$h-1;
  }

  # Get rid of fat walls
  my ($tries, $wall_fixed) = ($w*$h, 1);
  while ($tries-- > 0 && $wall_fixed) {
    $wall_fixed = undef;
    for my $i (1..$h-2) {
      for my $j (1..$w-3) {
        next unless wallp $bldg[$i][1];
        # Next line is wall or previous line is wall. And this line is wall.
        if (wallp($bldg[$i][$j+1]) &&
          (wallp($bldg[$i-1][$j]) && wallp($bldg[$i-1][$j+1]) ||
            (wallp($bldg[$i+1][$j]) && wallp($bldg[$i+1][$j+1])))) {
          $bldg[$i][$_] = $stuff{floor} for 1..$w-2;
          $wall_fixed = 1;
        }
      }
    }

    for my $i (1..$w-2) {
      for my $j (1..$h-3) {
        next unless wallp $bldg[1][$i];
        # Next line is wall or previous line is wall. And this line is wall.
        if (wallp($bldg[$j+1][$i]) &&
          (wallp($bldg[$j][$i-1]) && wallp($bldg[$j+1][$i-1]) ||
            (wallp($bldg[$j][$i+1]) && wallp($bldg[$j+1][$i+1])))) {
          $bldg[$_][$i] = $stuff{floor} for 1..$h-2;
        }
      }
    }

    # Fix leaky walls
    for my $i (1..$h-2) {
      for my $j (1..$w-2) {
        if (wallp $bldg[$i][$j]) {
          if (1 == wallp($bldg[$i-1][$j]) + wallp($bldg[$i+1][$j])) {
            # Try to fix broken vertical line
            # Find closest wall on the left
            my $wpos;
            for (reverse 1..$j-1) {
              $wpos = $_ and last if wallp $bldg[$i][$_];
            }
            if (defined $wpos) {
              # Conduct to the left
              $bldg[$i][$_] = $stuff{wall} for $wpos..$j-1;
              $wall_fixed = 1;
              next;
            }

            # Find closest wall on the right
            for ($j+1..$w-2) {
              $wpos = $_ and last if wallp $bldg[$i][$_];
            }
            if (defined $wpos) {
              # Conduct to the right
              $bldg[$i][$_] = $stuff{wall} for $j+1..$wpos;
              $wall_fixed = 1;
              next;
            }

            # Haven't found, make full vertical line
            $wall_fixed = 1;
            $bldg[$_][$j] = $stuff{wall} for 0..$h-1;
            next;
          }

          if (1 == wallp($bldg[$i][$j-1]) + wallp($bldg[$i][$j+1])) {
            # Try to fix broken horizontal line
            # Find closest wall on the top
            my $wpos;
            for (reverse 1..$i-1) {
              $wpos = $_ and last if wallp $bldg[$_][$j];
            }
            if (defined $wpos) {
              # Conduct to the top
              $bldg[$_][$j] = $stuff{wall} for $wpos..$i-1;
              $wall_fixed = 1;
              next;
            }

            # Find closest wall on the bottom
            for ($i+1..$h-2) {
              $wpos = $_ and last if wallp $bldg[$_][$j];
            }
            if (defined $wpos) {
              # Conduct to the bottom
              $bldg[$_][$j] = $stuff{wall} for $i+1..$wpos;
              $wall_fixed = 1;
              next;
            }

            # Haven't found, make full horizontal line
            $wall_fixed = 1;
            $bldg[$i][$_] = $stuff{wall} for 0..$w-1;
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
  $bldg[$edy][$edx] = $stuff{door};

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
