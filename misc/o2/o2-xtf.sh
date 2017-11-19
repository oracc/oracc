#!/bin/sh
scope=$1
echo entering o2-xtf.sh
# Build list before cancelling because it JSON depends
# on have-xtf.lst
o2-xtf-lst.sh

[ -r 01bld/cancel ] && exit 1

# make .txh files world-readable
umask 0022
shopt -s nullglob
project=`oraccopt`
rm -f 01tmp/xtfmanager.log

function has_score {
    for a in `cat 01bld/lists/sxh-scores.lst`; do
	f=`pqxpand xmd $a`
	xsltproc -o $f $ORACC/lib/scripts/has-score.xsl $f
    done
}

function has_sources {
    for a in `cat 01bld/lists/has-sources.lst`; do
	f=`pqxpand xmd $a`
	xsltproc -o $f $ORACC/lib/scripts/has-sources.xsl $f
    done
}

function xsf_scores {
    cat 01bld/lists/xsf-scores.lst >>01bld/lists/sxh-scores.lst
    for a in `cat 01bld/lists/xsf-scores.lst`; do
	g2-xsf2sxh.plx -proj $project $a
    done
}

p=`oraccopt`
if [[ $p == cdli* || $p == epsd2* ]]; then
    echo "USING LAZY HTML FOR PROJECT $p"
else
    xtfmanager.plx -list 01bld/lists/have-xtf.lst 2>01tmp/xtfmanager.log
    [ -s 01tmp/xtfmanager.log ] && wc -l 01tmp/xtfmanager.log
fi

o2-xtfindex.plx

if [ -r 01bld/lists/xtfindex.lst ]; then
    sort -t: -k2 -o 01bld/lists/xtfindex.lst 01bld/lists/xtfindex.lst
    wmapper -i 01bld/lists/xtfindex.lst -p $project
    cat 01bld/lists/xtfindex.lst | bigrams -p $project
fi

# linkbase
proj-linkbase.sh
xsltproc -o 01bld/lists/has-sources.lst $ORACC/lib/scripts/lst-sources.xsl  01bld/linkbase.xml

# scores
rm -f 01tmp/scoregen.log 01bld/lists/sxh-scores.lst
[ -s 01bld/lists/linktexts.lst ] && scoregen.plx 2>01tmp/scoregen.log | xmlsplit
# [ -s 01tmp/scoregen.log ] && wc -l 01tmp/scoregen.log
[ -s 01bld/lists/xsf-scores.lst ] && xsf_scores
[ -s 01bld/lists/sxh-scores.lst ] && has_score
[ -s 01bld/lists/has-sources.lst ] && has_sources

# indexes
dir=$ORACC/pub/$project

if [ -s 01bld/lists/lemindex.lst ]; then
    echo o2-xtf.sh: selemx ...
    sort -t: -k2 -o 01bld/lists/lemindex.lst 01bld/lists/lemindex.lst
    mkdir -p $dir/lem
    cat 01bld/lists/lemindex.lst | selemx -p $project
    sort -u -o 02pub/lem/mangle.tab 02pub/lem/mangle.tab
fi

mkdir -p $dir/tra
echo o2-xtf.sh: setrax ...
cat 01bld/lists/xtfindex.lst | setrax -p $project
sort -u -o 02pub/tra/mangle.tab 02pub/tra/mangle.tab

mkdir -p $dir/txt
echo o2-xtf.sh: setxtx ...
cat 01bld/lists/xtfindex.lst | setxtx -p $project
sort -u -o 02pub/txt/mangle.tab 02pub/txt/mangle.tab
sort -u -o 01tmp/signmap.x 01tmp/signmap.log
rm -f 01tmp/signmap.log

# must do this for EST project search
o2-xml.sh

# Other formats
if [ "$scope" = "full" ]; then
    tei=`oraccopt . build-tei`
    if [ "$tei" = "yes" ]; then
	o2-tei.sh
    fi
fi
#Q004184.70.1 

echo exiting o2-xtf.sh
