#!/bin/sh

## FIXME: this doesn't really work except for epsd2
project=`oraccopt`
o2-portal.sh
cp -p 00lib/alias-*.txt 02pub
if [ -d 01bld/snippets ]; then
    cp -a 01bld/snippets 02www
fi
pubfiles.sh $project
