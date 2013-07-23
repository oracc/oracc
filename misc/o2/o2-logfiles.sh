#!/bin/sh
LIST=`find 01tmp -follow -type f -name '*.log' -newer 01tmp/building`
for l in $LIST; do
    if [ -s $l ]; then
	echo '###'errors in $l:
	cat $l
    fi
done
