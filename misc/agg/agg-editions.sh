#!/bin/sh
if [ "$ORACC_MODE" != "single" ]; then
    oraccid=`id -u oracc`
    if [ "$EUID" != "$oraccid" ]; then
	echo aggregation can only be done by the 'oracc' user
	exit 1
    fi
fi

outxtf=$ORACC/agg/master-xtf.lst
rm -f $outxtf
for a in `agg-list-public-projects.sh` ; do
    if [ -r $ORACC/bld/$a/lists/have-xtf.lst ]; then
	cat $ORACC/bld/$a/lists/have-xtf.lst >>$outxtf
    fi
done
sort -u -o $outxtf $outxtf
chmod o+r $outxtf
