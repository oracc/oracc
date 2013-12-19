#!/bin/sh
xsltproc $ORACC/lib/scripts/issl-epsd.xsl $ORACC/bld/epsd2/sux/articles.xml >00src/epsd.raw
issl-epsd.plx
