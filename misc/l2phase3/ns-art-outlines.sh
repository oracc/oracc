#!/bin/sh
xsltproc ${ORACC_BUILDS}/lib/scripts/div-ids.xsl 01bld/www/cbd/sux/articles.html >01bld/sux/div-ids.tab
ns-art-outlines.plx
chmod o+r 01bld/sux/div-ids.tab 01bld/sux/templates.tab 01bld/sux/t*.html
