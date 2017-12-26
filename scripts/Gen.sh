#!/bin/sh
# Made by: KorG

set -e
cd "`dirname "$0"`"

pusk() {
   # echo "Running: $1" >&2
   shift
   perl "$@"
}

pusk "surface creation"       gen_surface.pl "$@"                      |
pusk "fields normalization"   gen_fields.pl                            |
pusk "fields correction"      gen_fields.pl -f                         |
pusk "castles creation"       gen_castle.pl                            |
pusk "final preparations"     gen_placeholders.pl                      |
less -S
