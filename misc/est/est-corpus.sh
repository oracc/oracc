#!/bin/sh
for a in `cut -d: -f2 01bld/lists/have-xtf.lst` ; do
    est-pqx.sh $a
done
