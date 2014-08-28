#!/bin/sh
function unscore {
    dir=`dirname $1`
    base=`basename $1 .xtf`
    xtf=$base.xtf
    xsf=$base.xsf
    (cd $dir ; \
	mv $xtf $xsf ; \
	echo converting $xsf to $xtf ... ; \
	echo `basename $xsf .xsf` >>../../lists/xsf-scores.lst ; \
	xsltproc $ORACC/lib/scripts/xtf-unscore.xsl $xsf >$xtf )
}
rm -f 01bld/lists/xsf-scores.lst
for a in `find 01bld/Q* -type f -name '*.xtf'`; do
    grep -l '<score' $a && unscore $a
done

