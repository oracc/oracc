#!/bin/sh
gunzip -cd 01bld/cdlicat.xmd.gz | xsltproc $ORACC/lib/scripts/tei-xmd-headers.xsl -
