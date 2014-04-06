#!/bin/sh
xsltproc -output $1 $ORACC/lib/scripts/gdf-sort.xsl $1
