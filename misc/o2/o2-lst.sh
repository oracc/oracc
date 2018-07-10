#!/bin/sh
mkdir -p 01bld/lists
rm -f 01bld/lists/cat-ids.lst
xmd-ids.plx 01bld/lists/cat-ids.lst
o2-lst.plx 2>lst.log
chmod o+r 01bld/lists/*
