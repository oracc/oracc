#!/bin/sh
webdir=01bld/www
project=`oraccopt`
prjdir=${ORACC_HOME}/$project
if [ ! -d $webdir ]; then
    echo o2-weblive.sh: no directory 01bld/www to make live
    exit 1
else
    (cd $webdir ; mv -f *.* $prjdir/02www ; \
	for a in * ; do rm -fr $prjdir/02www/$a ; mv -f $a $prjdir/02www ; done)
    p3-project.sh
    o2-prm.sh
fi
