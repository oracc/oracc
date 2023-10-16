#!/bin/sh
asl1=$1
asl2=$2

if [ "$asl1" == "" ]; then
    echo $0: must give two .asl files to compare. Stop.
    exit 1
elif [ "$asl2" == "" ]; then
    echo $0: must give two .asl files to compare. Stop.
    exit 1
fi

if [ ! -r "$asl1" ]; then
    echo $0: .asl file $asl1 not found. Stop.
elif [ ! -r "$asl2" ]; then
    echo $0: .asl file $asl2 not found. Stop.
fi

sxnorm.sh <$asl1 >$$-1.asl
sxnorm.sh <$asl2 >$$-2.asl

cmp $$-1.asl $$-2.asl || diff -a -w $$-[12].asl | less
