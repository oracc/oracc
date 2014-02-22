#!/bin/sh

# check for existing directories and exit if they exist
if [ -d 00web/00config ]; then
    echo esp2-create.sh: esp2 already created in 00web--remove before continuing.
    exit 1
fi

# make required directories
mkdir -p 00web/00config

# make home.xml from 00lib/config.xml data
if [ -r 00web/home.xml ]; then
    echo esp2-create.sh: will not overwrite 00web/home.xml--remove before continuing.
    exit 1
fi
xsltproc $ORACC/lib/scripts/esp2-home.xsl 00lib/config.xml >00web/home.xml
xsltproc $ORACC/lib/scripts/esp2-params.xsl 00lib/config.xml >00web/00config/parameters.xml
esp2-p3colours.plx >00web/00config/appearance.xml
cp $ORACC/lib/data/esp2-structure.xml 00web/00config/structure.xml
