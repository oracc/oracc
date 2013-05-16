#!/bin/sh

[ -r 01bld/cancel ] && exit 1

# make .txh files world-readable
umask 0022
shopt -s nullglob
project=`oraccopt`
rm -f 01tmp/xtfmanager.log
rm -f 01bld/lists/have-xtf.lst

function xsf_scores {
    for a in `cat 01bld/lists/xsf-scores.lst`; do
	g2-xsf2sxh.plx -proj $project $a
    done
}

o2-xtf-lst.sh
xtfmanager.plx -list 01bld/lists/have-xtf.lst 2>01tmp/xtfmanager.log
[ -s 01tmp/xtfmanager.log ] && wc -l 01tmp/xtfmanager.log

o2-xtfindex.plx

if [ -r 01bld/lists/xtfindex.lst ]; then
    sort -t: -k2 -o 01bld/lists/xtfindex.lst 01bld/lists/xtfindex.lst
    wmapper -i 01bld/lists/xtfindex.lst -p $project
    cat 01bld/lists/xtfindex.lst | bigrams -p $project
fi

# linkbase
proj-linkbase.sh

# scores
rm -f 01tmp/scoregen.log
[ -s 01bld/lists/linktexts.lst ] && scoregen.plx 2>01tmp/scoregen.log | xmlsplit
[ -s 01tmp/scoregen.log ] && wc -l 01tmp/scoregen.log
[ -s 01bld/lists/xsf-scores.lst ] && xsf_scores

# indexes
dir=$ORACC/pub/$project

if [ -s 01bld/lists/lemindex.lst ]; then
    sort -t: -k2 -o 01bld/lists/lemindex.lst 01bld/lists/lemindex.lst
    mkdir -p $dir/lem
    cat 01bld/lists/lemindex.lst | selemx -p $project
fi

mkdir -p $dir/tra
cat 01bld/lists/xtfindex.lst | setrax -p $project

mkdir -p $dir/txt
cat 01bld/lists/xtfindex.lst | setxtx -p $project

# Other formats
o2-xml.sh
tei=`oraccopt . build-tei`
if [ "$tei"=="yes" ]; then
    o2-tei.sh
fi
