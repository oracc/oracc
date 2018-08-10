#!/bin/sh
shopt -s nullglob
p=`oraccopt`
for a in 01bld/www/cbd/* ; do
    d=`basename $a`
    dbld=${ORACC_BUILDS}/$p/01bld/www/cbd/$d
    dwww=${ORACC_BUILDS}/$p/02www/cbd
    (cd $dwww ;
     echo Installing glossaries in `pwd`
     if [ -d $d ]; then
	 mv -v $d x$d ; mv -v $dbld . ; rm -fr x$d
     else
	 mv -v $dbld .
     fi)
done
