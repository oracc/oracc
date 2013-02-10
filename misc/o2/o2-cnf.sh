#!/bin/sh
if [ ! -r 00lib/config.xml ]; then
    echo oracc config must be run from a project directory
    exit 1
fi
if [ -r 00lib/subconfig.xml ]; then
    xsltproc -o 02xml/config.xml $ORACC/lib/scripts/subconfig.xsl \
	00lib/subconfig.xml
else
    cp -u 00lib/config.xml 02xml/config.xml
fi
cp -u 02xml/config.xml 02www
chmod -f o+r 02xml/config.xml 02www/config.xml
if [ -r 00lib/names.xml ]; then
    cp -u 00lib/names.xml 02pub
    chmod o+r 02pub/names.xml
fi
exit 0
