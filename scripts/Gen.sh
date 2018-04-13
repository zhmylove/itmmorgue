#!/bin/sh
# Made by: KorG

set -e
cd "`dirname "$0"`"

pusk() {
   # echo "Running: $1" >&2
   shift
   eval "./$@"
}

[ -z "$1" ] && set -- -w512 -h256

if [ "xSunOS" = "x`uname -s`" ] ;then
   cat SunOS.level
   exit 0
fi

pusk "surface creation"       gen_surface "$@"                      |
pusk "fields normalization"   gen_fields                            |
pusk "fields correction"      gen_fields -f                         |
#pusk "castles creation"       gen_castle                            |
#pusk "building buildings"     gen_buildings                         |
#pusk "building buildings"     gen_cities                            |
#pusk "final preparations"     gen_placeholders                      |
less -S
