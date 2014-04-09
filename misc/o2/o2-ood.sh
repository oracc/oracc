#!/bin/sh
project=`oraccopt`
if [ ! -r 00lib/data.xml ]; then
    echo o2-ood.sh: no 00lib/data.xml
    exit 1
fi
#xsltproc $ORACC/lib/scripts/gdf-index-html.xsl gdf.xml >index.html
mkdir -p $ORACC/pub/$project/cat
cp 00lib/data.xml 02pub
$ORACC/bin/segdfx -p $project < 02pub/data.xml
o2-web.sh

