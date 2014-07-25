#!/bin/sh
sl-xml.plx ogsl
sl-db.plx ogsl
sl-index -boot
xsltproc -stringparam project ogsl ${ORACC}/lib/scripts/sl-HTML.xsl 02xml/ogsl-sl.xml >02www/ogsl.html
cp 00web/ogsl.html 
cp -p 00lib/alias-*.txt 02pub
pubfiles.sh ogsl
