#!/bin/sh
shopt -s nullglob
project=`oraccopt . project`
PQX=`ls 01bld | grep -q '^[PQX]' && echo -n yes`
if [ "$PQX" = "yes" ]; then
    ls -1R 01bld/[PQX]* | ls1R2lst.plx \
	| perl -ne "s%^.*?/([PQX][0-9]{6}).*?$%$project:\$1%&&print" \
	>01bld/lists/have-xtf.lst
else
    cat </dev/null >01bld/lists/have-xtf.lst
fi

