#!/bin/sh
agg-test.sh || exit 1
outxtf=$ORACC/agg/master-xtf.lst
rm -f $outxtf
for a in `agg-list-public-projects.sh` ; do
    if [ -r $ORACC/bld/$a/lists/have-xtf.lst ]; then
	cat $ORACC/bld/$a/lists/have-xtf.lst >>$outxtf
    fi
done
sort -u -o $outxtf $outxtf
chmod o+r $outxtf
