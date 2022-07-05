#!/bin/sh
shopt -s nullglob
p=`oraccopt`
dwww=${ORACC_BUILDS}/$p/02www/cbd
mkdir -p $dwww
for a in 01bld/www/cbd/* ; do
    d=`basename $a`
    dbld=${ORACC_BUILDS}/$p/01bld/www/cbd/$d
    (cd $dwww ;
     echo Installing glossaries in `pwd`
     if [ -d $d ]; then
	 mv -v $d x$d ; mv -v $dbld . ; rm -fr x$d
     else
	 mv -v $dbld .
     fi)
done
if [ -d 01tmp/lex/cbd ]; then
    for l in 01tmp/lex/cbd/* ; do
	lang=`basename $l`
	echo l=$l
	for t in $l/* ; do
	    echo type=$t
	    mv -v $t $dwww/$lang
	done
    done
    cp 01tmp/lex/*provides*.xml 02www
fi

if [ -r 00etc/word-pages.lst ]; then
    esp2-word-pages.plx
fi

# if we are using OIDs we need to index the files
oid=`oraccopt . cbd-oid`
if [ "$oid" == "yes" ] ; then
    oid-index-cbd.sh `oraccopt`
fi
