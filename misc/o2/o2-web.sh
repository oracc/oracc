#!/bin/sh
[ -r 01bld/cancel ] && exit 1
shopt -s nullglob
webdir=01bld/www ; rm -fr $webdir
mkdir -p $webdir/t $webdir/cbd
cp -u 02xml/config.xml $webdir/config.xml
if [ -r 00lib/project.sty ]; then
    cp -p 00lib/project.sty $webdir/
fi
web-p2colours.plx $webdir
otl=`oraccopt . outline-special-list-name`
if [ "$otl" != "" ]; then
    if [ -r 00lib/$otl ]; then
	echo processing 00lib/$otl
	o2-list-default-project.sh `oraccopt` 00lib/$otl $webdir/lists/$otl
	chmod o+r $webdir/lists/$otl
    else
	echo no such file 00lib/$otl
    fi
fi
mkdir -p $webdir/lists
liblists=`echo -n 00lib/lists/*`;
if [ "$liblists" != "" ]; then
    cp -a 00lib/lists/* $webdir/lists
fi
for a in `ls 01bld/lists/* | egrep -v '.lst$'` ; do
    cp -fp $a $webdir/lists
done
if [ -r 01bld/lists/outlined.lst ]; then
    cp -fp 01bld/lists/outlined.lst $webdir/lists
fi
l2p3.sh $webdir
web-PQX-html.plx -list 01bld/lists/have-xtf.lst -webdir $webdir 2>01tmp/web-PQX.log
p2-project.sh
if [ -d 00web/esp ]; then
    # esp live includes o2-weblive
    oracc esp ; oracc esp live force
else
    [ -e 00web/index.html ] || web-default-index.plx
    mkdir -p $webdir/images
    cp -fpR 00web/* $webdir ; rm -f $webdir/*~
    o2-weblive.sh
fi
