#!/bin/sh
rm -fr browse
mkdir -p signlist/00lib signlist/00web/00config
cp -a 00lib/signlist-config.xml signlist/00lib/config.xml
cp -a 00lib/signlist-parameters.xml signlist/00web/00config/parameters.xml
xsltproc $ORACC/lib/scripts/sl-ESP-structure.xsl 02xml/sl-grouped.xml >signlist/00web/00config/structure.xml
xsltproc $ORACC/lib/scripts/sl-ESP-letters.xsl 02xml/sl-grouped.xml
xsltproc $ORACC/lib/scripts/sl-ESP-signs.xsl 02xml/sl-grouped.xml
(cd signlist ; o2-portal.sh)
