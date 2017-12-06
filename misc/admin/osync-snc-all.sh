#!/bin/sh
cd $ORACC_BUILDS
if [ ! -d snc ]; then
    echo "osync-snc-all.sh: $ORACC_BUILDS/snc doesn't exist. Stop."
    exit 1;
fi
for a in * ; do
    if [ -d $a/00lib ]; then
	if [ ! -f $a/.sync ]; then
	    osync-snc.sh $a
	else
	    echo skipping $a .sync
	fi
    else
	echo skipping $a no 00lib
    fi
done
