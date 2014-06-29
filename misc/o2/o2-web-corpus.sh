#!/bin/sh
[ -r 01bld/cancel ] && exit 1
p3-translangs.sh >01tmp/translangs
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
if [ -d 00lib/lists ]; then
    liblists=`(cd 00lib/lists ; ls)`
    if [ "$liblists" != "" ]; then
	cp -f 00lib/lists/* $webdir/lists
    fi
fi
if [ -d 01bld/lists ]; then
    bldlists=`(cd 01bld/lists ; ls)`
    if [ "$bldlists" != "" ]; then
	for a in `ls 01bld/lists/* | egrep -v '.lst$'` ; do
	    cp -f $a $webdir/lists
	done
    fi
fi
if [ -r 01bld/lists/outlined.lst ]; then
    cp -f 01bld/lists/outlined.lst $webdir/lists
fi
if [ -r 00lib/xmdoutline.xsl ]; then
    sed "s#@@ORACC@@#$ORACC#g" <00lib/xmdoutline.xsl >$webdir/xmdoutline.xsl
fi
l2p3.sh $webdir
for a in 02pub/cbd/* ; do 
    lang=`basename $a`
    if [ -r 01bld/$lang/letter_ids.tab ]; then
	cp 01bld/$lang/letter_ids.tab $a
	cp 01bld/$lang/L*.lst $a
    fi
done
if [ -f 01bld/lists/have-xtf.lst ]; then
    web-PQX-html.plx -list 01bld/lists/have-xtf.lst -proj `oraccopt` 2>01tmp/web-PQX.log
    clean-web-PQX-log.sh
fi
