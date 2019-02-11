#!/bin/sh
rm -fr signlist ; mkdir signlist
project=`oraccopt`
(cd $ORACC ;
    mkdir -p bld/$project/signlist
    for a in xml www pub ; do
	mkdir -p $a/$project/signlist
    done
)
(cd signlist ;
    mkdir -p 00lib ;
    mkdir -p 00res/css ;
    mkdir -p 00res/js ;
    mkdir -p 00web/00config ;
    mkdir -p 01tmp ;
    ln -sf $ORACC/bld/$project/signlist 01bld
    for a in xml www pub ; do
	ln -sf $ORACC/$a/$project/signlist 02$a
    done
    mkdir -p 01bld/www
)
cp -a 00lib/signlist-config.xml signlist/00lib/config.xml
cp -a 00lib/signlist-parameters.xml signlist/00web/00config/parameters.xml
cp -a 00lib/signlist-home.xml signlist/00web/home.xml
cp -a 00lib/slform.html signlist/00web/
cp -a 00lib/signlist-projesp.css signlist/00res/css/projesp.js
cp -a 00lib/signlist-sl.css signlist/00res/css/sl.css
cp -a 00lib/signlist-projesp.js signlist/00res/js/projesp.js
cp -a 00lib/signlist-sl.js signlist/00res/js/sl.js
xsltproc $ORACC/lib/scripts/sl-ESP-structure.xsl 02xml/sl-grouped.xml >signlist/00web/00config/structure.xml
xsltproc $ORACC/lib/scripts/sl-ESP-letters.xsl 02xml/sl-grouped.xml
xsltproc -stringparam project $project $ORACC/lib/scripts/sl-ESP-signs.xsl 02xml/sl-grouped.xml
(cd signlist ; o2-portal.sh)
if [ -r 00lib/signlist-index.html ]; then
    (cd signlist/02www ; mv index.html home.html)
    cp -af 00lib/signlist-index.html signlist/02www/index.html ; chmod o+r signlist/02www/index.html
fi
#sl-inner.sh
#sl-brief.sh
