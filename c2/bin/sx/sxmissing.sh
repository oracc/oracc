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
if [ -r $asl ]; then
    sx -n $asl >$dir/listnames.lst
    sx -M $asl >$dir/missing.out
    lists=`cat $dir/listnames.lst`
    for a in $lists; do
	grep $a'[0-9]' $dir/missing.out | pr -t -5 >$dir/missing-$a.txt
    done
    exit 0
else
    echo "$0: .asl file $asl not found or not readable. Stop."
    exit 1
fi
