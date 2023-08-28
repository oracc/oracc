#!/bin/sh
dir=$1
asl=$2
if [ "$dir" == "" ]; then
    if [ -d "00etc" ]; then
	dir="00etc"
    else
	dir="."
    fi
fi

if [ ! -d $dir ]; then
    echo "$0: $dir is not a directory. Stop."
    exit 1
fi

if [ "$asl" == "" ]; then
    asl=00lib/ogsl.asl
fi
sxudatacodes=$ORACC_BUILDS/lib/data/sx-udata-codes.tsv
if [ -r $asl ]; then
    sx -u $asl >$dir/u-data.tsv
    tables=`cut -f1 $sxudatacodes | grep -v need`
    for a in $tables; do
	grep ^$a $dir/u-data.tsv | cut -f2- | rocox -t >$dir/udata-$a.xml
    done
    grep ^need $dir/u-data.tsv | cut -f2 | pr -t -3 -w 200 -l 1000 | tr -s ' ' '\t' | rocox -t >$dir/udata-need.xml
    exit 0
else
    echo "$0: .asl file $asl not found or not readable. Stop."
    exit 1
fi
