#!/bin/sh
project=`oraccopt`
rm -f 02xml/corpus-sl.xml
sl-xml.plx `oraccopt` 01bld/csl.d/corpus.asl
sl-group.plx 02xml/corpus-sl.xml
xsltproc ${ORACC}/lib/scripts/sl-letters-file.xsl 02xml/sl-grouped.xml >02pub/letters.tab
(cd 02xml ; mv sl-grouped.xml sl-grouped-pre-sd.xml)
xsltproc 00bin/signdata-merge.xsl 02xml/sl-grouped-pre-sd.xml >02xml/sl-grouped.xml
xsltproc 00bin/snippet-pages.xsl 00etc/snippets.xml
sl-signlist.sh with-stats
(cd 02xml ; ln -sf sl-grouped.xml corpus-sl.xml)
sl-db.plx corpus 2>log
sl-index corpus <02pub/sl/corpus-db.tsv
chmod o+r 02pub/sl/*
