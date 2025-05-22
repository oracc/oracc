#!/bin/sh
project=`oraccopt`
if [ "$project" = "" ]; then
    echo o2-prm.sh: must be run from a project directory
    exit 1
fi
odo-chmod.sh -R go+r ${ORACC}/{bld,pub,www,xml}/$project
