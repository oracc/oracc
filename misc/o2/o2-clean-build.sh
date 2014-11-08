#!/bin/sh
rm -f 01bld/cancel
o2-unbuild.sh
touch 01tmp/building
o2-rebuild.sh $*
o2-logfiles.sh
rm -f 01tmp/building
