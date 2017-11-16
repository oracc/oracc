#!/bin/sh
mkdir -p 01bld/cat
atflinks.plx >01bld/atflinks.tab
linknorm.plx 01bld/atflinks.tab >01bld/normlinks.tab
translist.sh >01bld/translist.txt
xmdmanager2.plx
xmd-check.plx
xsltproc ${ORACC}/lib/scripts/xmd-summaries.xsl 01bld/cdlicat.xmd >01bld/cat-sum.xml
xsltproc ${ORACC}/lib/scripts/xmd-HTML-summaries.xsl 01bld/cat-sum.xml >01bld/cat-sum.html
