#!/bin/sh
project=`oraccopt`
if [ ! -r 00lib/data.xml ]; then
    echo o2-ood.sh: no 00lib/data.xml
    exit 1
fi
echo $0 calling ood-one.sh in `pwd`
ood-one.sh
