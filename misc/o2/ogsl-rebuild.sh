#!/bin/sh
sl-xml.plx ogsl
sl-group.plx
xsltproc ${ORACC}/lib/scripts/sl-letters-file.xsl 02xml/sl-grouped.xml >02pub/letters.tab
sl-signlist.sh
sl-db.plx ogsl
sl-index -boot
xsltproc -stringparam project ogsl ${ORACC}/lib/scripts/sl-HTML.xsl 02xml/ogsl-sl.xml >02www/ogsl.html
cp -p 00lib/alias-*.txt 02pub
pubfiles.sh ogsl
