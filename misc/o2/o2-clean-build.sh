#!/bin/sh
if [ -r .nobuild ]; then
    echo 'You are not allowed to build this project here. Stop.'
    exit 1
fi
rm -f 01bld/cancel
o2-unbuild.sh
touch 01tmp/building
o2-rebuild.sh $*
o2-logfiles.sh
rm -f 01tmp/building
