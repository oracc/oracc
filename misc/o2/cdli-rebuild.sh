#!/bin/sh
#
# As of 2021-01-20 this just rebuilds catalog data not corpus
#
# Adapted 2021-11-25 to work with CDLI's move to git lfs
#
d=`isogmt`
echo Rebuilding CDLI with cdli-rebuild.sh ...
(cd 00git/data ; git pull)
if [ $? == 1 ]; then
    head -1 00git/data/cdli_cat.csv >00lib/fieldnames.row
    00bin/csv-clean.plx
    o2-catalog.sh
    exit 0
else
    if [ $? == 0 ]; then
	echo "$0 $d: git up to date, no rebuild or serve necessary."
    else
	echo "$0 $d: exiting on git error, no rebuild or serve."
    fi
    exit $?
fi
