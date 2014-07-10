#!/bin/sh
shopt -s nullglob
project=`oraccopt . project`
PQX="01bld/[PQX]*"
if [ -s "$PQX" ]; then
    echo searching $PQX for .xtf files ...
    ls -1R 01bld/[PQX]* | ls1R2lst.plx \
	| perl -ne "s%^.*?/([PQX][0-9]{6}).*?$%$project:\$1%&&print" \
	>01bld/lists/have-xtf.lst
    ls -1R 01bld/[PQX]* | ls1R2lst.plx xsf \
	| perl -ne "s%^.*?/([PQX][0-9]{6}).*?$%$project:\$1%&&print" \
	>01bld/lists/xsf-scores.lst
else
    rm -f 01bld/lists/have-xtf.lst
    touch 01bld/lists/have-xtf.lst
fi
