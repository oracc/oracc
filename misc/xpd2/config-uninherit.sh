#!/bin/sh
if [ ! -r 00lib/subconfig.xml ]; then
    echo config-uninherit.sh: no 00lib/subconfig.xml to uninherit
    exit 1
fi
if [ ! -r 02xml/config.xml ]; then
    echo config-uninherit.sh: no 02xml/config.xml to uninherit with
    exit 1
fi
rm -f 00lib/subconfig.xml 00lib/config.xml
xsltproc $ORACC/lib/scripts/identity.xsl 02xml/config.xml >00lib/config.xml
