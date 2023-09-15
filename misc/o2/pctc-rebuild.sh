#!/bin/sh
if [ -r .nobuild ]; then
    echo 'You are not allowed to build pctc here; it can only be built on build-oracc. Stop.'
    exit 1
else
    echo 'Running pctc-rebuild.sh'
fi
o2-asl.sh
