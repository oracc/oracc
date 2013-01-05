#!/bin/sh
libbib=/usr/local//lib/bib
echo '<wrapper>' >wrapped.tei
perl -p -e 's/^<\?xml.*\?>$//' $* >>wrapped.tei
echo '</wrapper>' >>wrapped.tei
xsltproc $libbib/tei-merge.xsl wrapped.tei >merged.tei
xsltproc $libbib/tei-xmlid.xsl merged.tei >withids.tei
