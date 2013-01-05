#!/bin/sh
if [ "$1" = "" ]; then
    echo pubfiles.sh: must give project name as argument
    exit 1
fi
chmod -R go+r ${ORACC}/{bld,pub,www,xml}/$1
