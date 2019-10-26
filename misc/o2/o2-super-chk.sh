#!/bin/sh
# For any project in 00lib/superdyn.lst update the map file
# dynamically
rm -f .supercancel
superglo=$1
if [ "$superglo" == "" ]; then
    echo "$0: must give name of super-glossary, e.g., 01tmp/sux.glo"
    exit 1
fi
if [ -r 00lib/superchk.lst ]; then
    cbd-super-compare.plx -dynamic -base $superglo -list 00lib/superchk.lst
    if [ -r '.supercancel' ]; then
	echo "o2-super-chk.sh: check via 00lib/superchk.lst against $superglo failed"
	exit 1
    fi
    cbd-super-map-tab.plx
    exit 0
else
    echo "$0: no 00lib/superchk.lst, nothing to do."
    exit 1
fi
