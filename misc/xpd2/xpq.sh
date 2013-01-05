#!/bin/sh
if [ "$1" = "" ]; then
    echo expand-pq.sh: must give project name as first argument
    echo expand-pq.sh: e.g.: expand-pq.sh dcclt xtf
    exit 1
fi
if [ "$2" = "" ]; then
    echo expand-pq.sh: must give file extension as second argument
    echo expand-pq.sh: e.g.: expand-pq.sh dcclt xtf
    exit 1
fi
while read line ; do \
    proj=`echo $line | cut -d: -f1` ; \
    text=`echo $line | cut -d: -f2` ; \
    if [ "$proj" == "$text" ]; then
      echo $ORACC/bld/$1/${line:0:4}/$line/$line.$2
    else
      echo $ORACC/bld/$proj/${text:0:4}/$text/$text.$2
    fi
done
