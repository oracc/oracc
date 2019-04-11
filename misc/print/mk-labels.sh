#!/bin/sh
rm -f new-labels.tab
for a in `perl ~/oracc/misc/print/mk-labels.plx | pqxpand xtf` ; do
    xsltproc $ORACC_BUILDS/lib/scripts/mk-labels.xsl $a >>new-labels.tab
done

