#!/bin/sh
stats=$*
rm -fr csl ; mkdir csl
project=`oraccopt`
(cd $ORACC ;
    mkdir -p bld/$project/csl
    for a in xml www pub ; do
	mkdir -p $a/$project/csl
    done
)
(cd csl ;
    mkdir -p 00lib ;
    mkdir -p 00res/css ;
    mkdir -p 00res/js ;
    mkdir -p 00web/00config ;
    mkdir -p 01tmp ;
    ln -sf $ORACC/bld/$project/csl 01bld
    for a in xml www pub ; do
	ln -sf $ORACC/$a/$project/csl 02$a
    done
    mkdir -p 01bld/www
)
cp -a 00lib/csl-config.xml csl/00lib/config.xml
cp -a 00lib/csl-parameters.xml csl/00web/00config/parameters.xml
cp -a 00lib/csl-home.xml csl/00web/home.xml
cp -a 00lib/slform.html csl/00web/
cp -a 00lib/csl-projesp.css csl/00res/css/projesp.css
cp -a 00lib/csl-sl.css csl/00res/css/sl.css
cp -a 00lib/csl-projesp.js csl/00res/js/projesp.js
cp -a 00lib/csl-sl.js csl/00res/js/sl.js
xsltproc $ORACC/lib/scripts/sl-ESP-structure.xsl 02xml/sl-grouped.xml >csl/00web/00config/structure.xml
xsltproc $ORACC/lib/scripts/sl-ESP-letters.xsl 02xml/sl-grouped.xml
sl-signdata.plx $project
(cd 02xml ; mv sl-grouped.xml sl-grouped-pre-sd.xml)
xsltproc $ORACC/lib/scripts/sl-signdata-merge.xsl 02xml/sl-grouped-pre-sd.xml >02xml/sl-grouped.xml
if [ "$stats" = "with-stats" ]; then
    echo with-stats=true
    xsltproc -stringparam with-stats yes -stringparam project $project $ORACC/lib/scripts/sl-ESP-signs.xsl 02xml/sl-grouped.xml
else 
    xsltproc -stringparam project $project $ORACC/lib/scripts/sl-ESP-signs.xsl 02xml/sl-grouped.xml
fi
(cd csl ; o2-portal.sh)
if [ -r 00lib/csl-index.html ]; then
    (cd csl/02www ; mv index.html home.html)
    cp -af 00lib/csl-index.html csl/02www/index.html ; chmod o+r csl/02www/index.html
fi
#sl-inner.sh
#sl-brief.sh
