#!/bin/sh
rm -f labels.tab
for a in `perl $ORACC_BUILDS/bin/mk-labels.plx $* | pqxpand xtf` ; do
    xsltproc $ORACC_BUILDS/lib/scripts/mk-labels.xsl $a >>labels.tab
done
