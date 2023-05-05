#!/bin/sh
cat $1 | j2x | xsltproc $ORACC_BUILDS/lib/scripts/j2x-uptag.xsl -
