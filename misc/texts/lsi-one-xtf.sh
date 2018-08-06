#!/bin/sh
out=01tmp/lsi/`basename $1 .xtf`.lsi
xsltproc -o $out $ORACC_BUILDS/lib/scripts/lsi-get-data.xsl $1
