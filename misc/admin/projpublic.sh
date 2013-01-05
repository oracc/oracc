#!/bin/sh
for a in `allprojlist.sh $1`; do
    p=`oraccopt $a public`
    if [ "$p" == "yes" ]; then
	echo $a
    fi
done
