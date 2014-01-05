#!/bin/sh
if [ -r 01bld/cdlicat.xmd.gz ]; then
    gunzip -cd 01bld/cdlicat.xmd.gz | xsltproc $ORACC/lib/scripts/tei-xmd-headers.xsl -
else
    xsltproc $ORACC/lib/scripts/tei-xmd-headers.xsl 01bld/cdlicat.xmd
fi
