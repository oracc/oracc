#!/bin/sh
mkdir -p 01bld/lists
rm -f 01bld/lists/cat-ids.lst
xmd-ids.plx 01bld/lists/cat-ids.lst
o2-lst.plx
if [ -r 00lib/umbrella.lst ] || [ -r 00lib/search.lst ]; then
    lx-lists.sh
fi
chmod o+r 01bld/lists/*
