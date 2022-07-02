#!/bin/sh
in=$1
out=$2
echo "$0: converting $in to $out"
grep -v '!DOCTYPE' $in | xsltproc --xinclude $ORACC_BUILDS/lib/scripts/art-HTML-div.xsl - >$out
