#!/bin/sh
project=`oraccopt`

function xis {
    if [ -r $ORACC_BUILDS/pub/$project/sortinfo.csi ]; then
	echo running xisperiods -t 02pub/cbd/$l/$l.tis -x 01bld/$l/$l.xis -p $project ...
	xisperiods -t 02pub/cbd/$l/$l.tis -x 01bld/$l/$l.xis -p $project >01bld/$l/periods.xis
	[ -s 01bld/$l/periods.xis ] || cp $ORACC/lib/data/dummy.xis 01bld/$l/periods.xis
	(cd 01bld/$l ; \
	 mv $l.xis $l.xis.top ; \
	 xsltproc -stringparam sub $l.xis.sub $ORACC/lib/scripts/l2p2-xis-merge.xsl $l.xis.top >$l.xis
	)
    else
	cp $ORACC/lib/data/dummy.xis 01bld/$l/periods.xis
    fi
}

function cbd {
    if [ -r 01bld/$l.glo.norm ]; then
	echo creating $ldir/$l.cbd via 01bld/$l.glo.norm ...
	l2-glomanager.plx -xml 01bld/$l.glo.norm -out $ldir/$l.cbd
    elif [ -r 00lib/$l.glo ]; then
	echo creating $ldir/$l.cbd from 00lib/$l.glo ...
	l2-glomanager.plx -xml 00lib/$l.glo -out $ldir/$l.cbd
    fi
}

function g2x {
    echo creating $ldir/$l.g2x from $ldir/union.sig
    withall=`oraccopt . cbd-with-all`
    if [ "$withall" = "yes" ]; then
	echo l2p2-g2x.plx -all -h $ldir/union.sig
	l2p2-g2x.plx -all -h $ldir/union.sig
    else
	echo l2p2-g2x.plx -h $ldir/union.sig
	l2p2-g2x.plx -h $ldir/union.sig
    fi
    xis $ldir $l
    if [ -s $ldir/$l.map ]; then
#	echo running l2p2-g2c.plx $l
	l2p2-g2c.plx $l
	export ORACC_HOME
	l2-glomanager.plx -conf l2p2.xcf -cbdlang $l
    else
	echo Glossary $l is empty.  Exterminate.
    fi
}

rm -f 01bld/cancel
projtype=`oraccopt . type`
super=`oraccopt . cbd-super`
echo projtype=$projtype
if [ "$projtype" == "superglo" ]; then
    for a in `oraccopt . cbd-super-list` ; do
	ldir=01bld/$a
	l=$a
	mkdir -p $ldir
	cbd $l $ldir
	echo l2p2.sh: processing sigs for superglo $a
	rm -f $ldir/union.sig
	cat 01bld/from-prx-*.sig >$ldir/$l.sig
	l2-sig-union.plx -super -proj $project -lang $l $ldir/glossary.sig $ldir/$l.sig >$ldir/union.sig
	g2x $ldir $l
    done
elif [ "$super" == "" ]; then
    for l in `l2p2-langs.plx` ; do
	ldir=01bld/$l
	mkdir -p $ldir
	cbd $l
    done
    if [ -e 01bld/cancel ]; then
	echo REBUILD CANCELLED
	exit 1
    fi
    for l in `l2p2-langs.plx` ; do
	ldir=01bld/$l
	mkdir -p $ldir
	rm -f $ldir/union.sig
	[ -r 01bld/project.sig ] && l2p2-sig-slicer.plx -lang $l
	[ -r 01bld/from-glos.sig ] && l2p2-sig-slicer.plx -lang $l -name glossary -sigs 01bld/from-glos.sig
	if [ -r $ldir/glossary.sig ] && [ -r $ldir/$l.sig ]; then
	    l2-sig-union.plx $ldir/$l.sig $ldir/glossary.sig >$ldir/union.sig
	elif [ -r $ldir/glossary.sig ]; then
	    #	    (cd $ldir ; ln -sf glossary.sig union.sig)
	    l2-sig-union.plx $ldir/glossary.sig >$ldir/union.sig
	else
	    #	    (cd $ldir; ln -sf $l.sig union.sig)
	    l2-sig-union.plx $ldir/$l.sig >$ldir/union.sig
	fi
	if [ -s $ldir/union.sig ]; then
	    g2x $ldir $l
	else
	    (cd $ldir; ln -sf $l.cbd $l.g2c; ln -sf $ORACC/lib/data/dummy.xis periods.xis)
	    l2-glomanager.plx -conf l2p2.xcf -cbdlang $l
	    l2p2-letters.plx $l
	fi
    done
else
    l2p2-union.plx
    for a in `ls 01bld/*/union.sig` ; do
	ldir=`dirname $a`
	l=`basename $ldir`
	g2x $ldir $l
    done
fi

if [ -r 01bld/pleiades.tab ]; then
    cp 01bld/pleiades.tab 02pub
fi

#usages=`oraccopt . cbd-usages`
#if [ "$usages" = "yes" ]; then
#    if [ "$project" = "epsd2" ]; then
#need to port this over to new build organization
#	epsd2-usages.sh
#    else
#	echo l2p2.sh: "cbd-usages only works with epsd2 at the moment--tell Steve to fix this!"
#    fi
#fi
