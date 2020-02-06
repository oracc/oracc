#!/bin/sh
pq=$1
scripts=$ORACC_BUILDS/lib/scripts
xtf=`xsltproc $scripts/oss-l-ids.xsl $pq.xtf`
xst=`xsltproc $scripts/oss-l-ids.xsl $pq.xst`
if [ "$xtf" != "$xst" ]; then
    echo "$pq: lemma IDs don't match in .xtf/.xst"
    exit 1
fi
exit 0
