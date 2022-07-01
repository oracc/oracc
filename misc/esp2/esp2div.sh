#!/bin/sh
in=$1
out=$2
xsltproc $ORACC_BUILDS/lib/scripts/esp-HTML-div.xsl $in >$out
