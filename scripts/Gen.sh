#!/bin/sh
# Made by: KorG

pusk() {
   echo "Running: $1" >&2
   shift
   perl "$@"
}

pusk "surface creation"       gen_surface.pl -w256 -h64                |
pusk "fields normalization"   gen_fields.pl                            |
pusk "fields correction"      gen_fields.pl -f                         |
pusk "castles creation"       gen_castle.pl                            |
pusk "final preparations"     gen_placeholders.pl                      |
less -S
