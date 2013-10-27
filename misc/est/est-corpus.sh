#!/bin/sh
if [ -r 01bld/lists/have-xtf.lst ]; then
    for a in `cut -d: -f2 01bld/lists/have-xtf.lst` ; do
	est-pqx.sh $a
    done
fi
