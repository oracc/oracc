#!/bin/sh

project=$1
if [ "$project" == "" ]; then
    echo "$0: must give project on command line. Stop."
    exit 1
fi

projdir=$ORACC_BUILDS/$project
cd $projdir

if [ -r 00lib/names.tab ]; then
    bki-names-xml.sh
else
    echo "$0: must provide 00lib/names.tab (ID<TAB>NAME) to use in indexing. Stop."
    exit 1    
fi

mkdir -p 01tmp/bki/qpn ; cp 00lib/names.xml 01tmp/bki/qpn ; cd 01tmp/bki
lib=$ORACC_BUILDS/lib/scripts
(cd qpn ; 
 bki-HTML.sh $projdir/01bld/qpn/articles.xml $projdir/01bld/qpn/qpn.xis
 bki-merge-xrr.sh bki-sorted+++.xml
 bki-split-qpn.plx bki-flat-rr.xml
 (cd qpn-bits ; \
     for a in qpn-x-*.xml ; do
	 h=`basename $a .xml`.html 
	 xsltproc $ORACC_BUILDS/lib/scripts/bki-chresto-HTML.xsl $a >$h 
     done
     )
 )
zip -j qpn-indices qpn/qpn-bits/qpn-x-*.html $ORACC_BUILDS/www/css/chresto-index.css
cp qpn-indices.zip $ORACC_BUILDS/www/$project/downloads
chmod o+r $ORACC_BUILDS/www/$project/downloads
cd $ORACC_BUILDS/$project ; o2-index-downloads.sh
