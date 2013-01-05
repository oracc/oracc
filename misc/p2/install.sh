#!/bin/sh
cp -uf *.css ~/www/css
cp -uf *.js ~/www/js
chmod o+r ~/www/js/* ~/www/css/*
mkdir -p ~/lib/ORACC/P2
cp -uf Util.pm ~/lib/ORACC/P2
cp -uf p2*.plx p2*.sh ~/bin
cp -uf p2-base.xml p2colours.txt  ~/lib/data
cp -uf pll_linkids.xml ~/lib/data/
cp -uf *.xsl ~/lib/scripts
mkdir -p ~/www/p2 ; cp p2*.html as-base.xml ~/www/p2 ; chmod -R o+r ~/www/p2
xsltproc -stringparam instance browse default-instance.xsl p2-base.xml >~/lib/data/default-browse.xml
xsltproc -stringparam instance pager default-instance.xsl p2-base.xml >~/lib/data/default-pager.xml
xsltproc -stringparam instance search default-instance.xsl p2-base.xml >~/lib/data/default-search.xml
chmod o+r ~/lib/ORACC/P2/Util.pm ~/bin/p2*.plx ~/lib/data/default-*.xml ~/bin/p2*.sh
chmod o+r ~/lib/data/*-base.xml ~/lib/data/p2colours.txt ~/lib/data/pll_linkids.xml ~/lib/scripts/*.xsl
chmod ugo+rx ~/bin/p2*
