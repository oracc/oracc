#!/bin/sh
# For any project in 00lib/superdyn.lst update the map file
# dynamically
superglo=$1
if [ "$superglo" == "" ]; then
    echo "$0: must give name of super-glossary, e.g., 01tmp/sux.glo"
    exit 1
fi
if [ -r 00lib/superdyn.lst ]; then
    ext=0
    for a in `cat 00lib/superdyn.lst`; do
	echo Mapping $a into $superglo
	ext=$((1+$ext))
	cbd-super-compare.plx -apply -dynamic -incr $ext -inplace -base $superglo -proj $a
	if [ -r '.supercancel' ]; then
	    echo "o2-super-dyn.sh: failed to map $a into $superglo (incr=$ext)"
	    exit 1
	fi
    done
    superlang=`basename $superglo .glo`
    for m in 01map/*.map ; do
	cbd-super-map-sigs.plx -lang $superlang -auto $m
    done
else
    echo "$0: no 00lib/superdyn.lst, nothing to do."
fi
