#!/bin/sh
#
# As of 2021-01-20 this just rebuilds catalog data not corpus
#
echo Rebuilding CDLI with cdli-rebuild.sh ...
(cd 00git ; git pull)
if [ $? == 1 ]; then
    head -1 00git/cdli_catalogue_1of2.csv >00lib/fieldnames.row
    00bin/csv-clean.plx
    o2-catalog.sh
    exit 0
else
    d=`isogmt`
    if [ $? == 0 ]; then
	echo "$0 $d: git up to date, no rebuild or serve necessary."
    else
	echo "$0 $d: exiting on git error, no rebuild or serve."
    fi
    exit $?
fi
