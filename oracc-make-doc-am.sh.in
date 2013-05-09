#!/bin/sh
x=`pwd`
#cd @@ORACC@@/src/doc
#echo SUBDIRS = >Makefile.am
#for a in `find . -name '*.xdf'` ; do 
#    (d=`dirname $a`; echo '	'$d' \' >>Makefile.am ; \
#	cd $d ; pwd ; @@ORACC@@/src/oracc-doc-am.plx `basename $a` )
#done

for a in `find . -name '*.xdf'` ; do 
    (d=`dirname $a`; cd $d ; $x/oracc-doc-am.plx `basename $a` )
done
