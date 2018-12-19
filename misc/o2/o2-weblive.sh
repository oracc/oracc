#!/bin/sh
webdir=01bld/www
project=`oraccopt`
prjdir=${ORACC_HOME}/$project
if [ ! -d $webdir ]; then
    echo o2-weblive.sh: no directory 01bld/www to make live
    exit 1
else
    if [ -r 00lib/xmdoutline.xsl ]; then
	sed "s#@@ORACC@@#$ORACC#g" <00lib/xmdoutline.xsl >$webdir/xmdoutline.xsl
    fi
    p3-colours.plx $webdir
#    (cd $webdir ; mv -f *.* $prjdir/02www ; \
    #	for a in * ; do rm -fr $prjdir/02www/$a ; mv -f $a $prjdir/02www ; done)
    l2p3-install-cbd.sh
    o2-weblive.plx | /bin/sh -s
    p3-project.sh
    o2-index-downloads.sh
    o2-prm.sh
fi
