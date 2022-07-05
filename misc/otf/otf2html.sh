#!/bin/sh
in=$1
out=$2
ox $in | xsltproc $ORACC_BUILDS/lib/scripts/otf-HTML.xsl >$out
