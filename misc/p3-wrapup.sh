#!/bin/sh
if [ -r 02www/news/index.html ]; then
    xmllint --output 02www/news/index.html --html --dropdtd --xmlout 02www/news/index.html
fi
xsltproc $ORACC/lib/scripts/p3-social.xsl 02xml/config.xml >02www/s.html
chmod o+r 02www/s.html
