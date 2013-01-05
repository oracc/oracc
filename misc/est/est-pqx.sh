#!/bin/sh
xsltproc \
    -stringparam oracc $ORACC \
    -stringparam project `oraccopt` \
    -stringparam item $1 \
    $ORACC/lib/scripts/est-PQX.xsl \
    $ORACC/lib/data/empty.xml \
    |$ORACC/bin/est-clean-keys.plx \
    |tr -s '-' >01bld/est/$1.est
