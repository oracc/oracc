#!/bin/sh
shopt -s nullglob
project=`oraccopt . project`
PQX="01bld/[PQX]*"
if [ "$PQX" != "" ]; then
    ls -1R 01bld/[PQX]* | ls1R2lst.plx \
	| perl -ne "s%^.*?/([PQX][0-9]{6}).*?$%$project:\$1%&&print" \
	>01bld/lists/have-xtf.lst
    ls -1R 01bld/[PQX]* | ls1R2lst.plx xsf \
	| perl -ne "s%^.*?/([PQX][0-9]{6}).*?$%$project:\$1%&&print" \
	>01bld/lists/xsf-scores.lst
else
    cat </dev/null >01bld/lists/have-xtf.lst
fi
