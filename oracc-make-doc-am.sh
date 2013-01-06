#!/bin/sh
cd /usr/local/oracc/src/doc
#echo SUBDIRS = >Makefile.am
#for a in `find . -name '*.xdf'` ; do 
#    (d=`dirname $a`; echo '	'$d' \' >>Makefile.am ; \
#	cd $d ; pwd ; /usr/local/oracc/src/oracc-doc-am.plx `basename $a` )
#done

for a in `find . -name '*.xdf'` ; do 
    (d=`dirname $a`; cd $d ; /usr/local/oracc/src/oracc-doc-am.plx `basename $a` )
done
