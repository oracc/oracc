#!/bin/sh
#grep -h '^&' 00atf/*.atf | perl -p -e 's/^.\s*(\S+)\s*=\s*/$1\t/' >00lib/names.tab
mkdir -p 01tmp
xsltproc $ORACC_BUILDS/lib/scripts/bki-names-tab.xsl 01bld/cdlicat.xmd >01tmp/names.tab
if [ -r 00lib/names.tab ]; then
    echo "Will not overwrite existing 00lib/names.tab"
    echo "New names.tab has been written to 01tmp/names.tab; review and update 00lib/names.tab"
else
    mv 01tmp/names.tab 00lib/names.tab
    echo "Created 00lib/names.tab"
fi

