#!/bin/sh
if [ -r 01tmp/estindex.log ]; then
    mv 01tmp/estindex.log 01tmp/estindex.logx
    grep -v INFO 01tmp/estindex.logx >01tmp/estindex.log
fi
rm -f 01tmp/zip
LIST=`find 01tmp -follow -type f -name '*.log' -newer 01tmp/building`
for l in $LIST; do
    if [ -s $l ]; then
	wc -l $l
#	echo '###'errors in $l:
#	cat $l | grep -v 'failed.*xtr'
    fi
done
