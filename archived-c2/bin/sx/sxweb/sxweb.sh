#!/bin/sh

project=`oraccopt`

if [ "$project" == "" ]; then
    echo "$0: must be called in a project directory. Stop."
    exit 1
fi

abbrev=`oraccopt . abbrev`
hproject=`/bin/echo -n $project | tr / -`
libdata=$ORACC_BUILDS/lib/data

function sxinst {    
    sed "s/@@PROJECT@@/$abbrev/g" $libdata/$1 \
	| sed "s/@@project@@/$project/g" \
	| sed "s/@@hproject@@/$hproject/g" \
	      >$2
}

stats=$*

rm -fr signlist ; mkdir signlist

(cd $ORACC_BUILDS ;
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
    ln -sf $ORACC_BUILDS/bld/$project/signlist 01bld
    for a in xml www pub ; do
	ln -sf $ORACC_BUILDS/$a/$project/signlist 02$a
    done
    mkdir -p 01bld/www
)

sxinst signlist-config.xml signlist/00lib/config.xml
sxinst signlist-parameters.xml signlist/00web/00config/parameters.xml
sxinst signlist-home.xml signlist/00web/home.xml
sxinst signlist-slform.html signlist/00web/slform.html
sxinst signlist-projesp.css signlist/00res/css/projesp.css
sxinst signlist-sl.css signlist/00res/css/sl.css
sxinst signlist-projesp.js signlist/00res/js/projesp.js
sxinst signlist-sl.js signlist/00res/js/sl.js

cp -f 00lib/signlist-x-*.xml signlist/00web

libscripts=$ORACC_BUILDS/lib/scripts

xmllint --xinclude - < $libscripts/sxweb-structure.xsl >01tmp/sxweb-structure-xi.xsl

xsltproc 01tmp/sxweb-structure-xi.xsl 02xml/sl.xml >signlist/00web/00config/structure.xml

xsltproc $libscripts/sxweb-letters.xsl 02xml/sl.xml

xsltproc $libscripts/sxweb-atoms.xsl 02xml/sl.xml \
    | xsltproc $libscripts/sxweb-overview.xsl - >signlist/00web/overview.xml

if [ "$stats" = "with-stats" ]; then
    echo with-stats=true
    xsltproc -stringparam with-stats yes -stringparam project $project $libscripts/sxweb-signs.xsl 02xml/sl.xml
else 
    xsltproc -stringparam project $project $libscripts/sxweb-signs.xsl 02xml/sl.xml
fi

(cd signlist ; o2-portal.sh)
