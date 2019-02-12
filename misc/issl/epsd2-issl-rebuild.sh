#!/bin/sh
o2-cnf.sh
issl-epsd.sh
issl-xml.plx 00src/*.txt
mkdir -p 01bld/sux 02pub/cbd/sux
xsltproc $ORACC/lib/scripts/issl-cbdx.xsl 01bld/issl.xml >01bld/sux/articles.xml
secbdx -p epsd2/issl -l sux 01bld/sux/articles.xml
o2-web.sh
mkdir -p 02www/cbd/sux
xsltproc $ORACC/lib/scripts/issl-summary.xsl 01bld/issl.xml >02www/cbd/sux/summaries.html
o2-prm.sh
