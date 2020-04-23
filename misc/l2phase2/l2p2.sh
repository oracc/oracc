#!/bin/sh
project=`oraccopt`

function xis {
    if [ -r $ORACC_BUILDS/pub/$project/sortinfo.csi ]; then
#	echo running xisperiods -t 02pub/cbd/$l/$l.tis -x 01bld/$l/$l.xis -p $project ...
	xisperiods -t 02pub/cbd/$l/$l.tis -x 01bld/$l/$l.xis -p $project >01bld/$l/periods.xis
	[ -s 01bld/$l/periods.xis ] || cp $ORACC/lib/data/dummy.xis 01bld/$l/periods.xis
	(cd 01bld/$l ; \
	 mv $l.xis $l.xis.top ; \
	 xsltproc -stringparam sub $l.xis.sub $ORACC/lib/scripts/l2p2-xis-merge.xsl $l.xis.top >$l.xis
	)
#	lex-data-tis.sh
    else
	cp $ORACC/lib/data/dummy.xis 01bld/$l/periods.xis
    fi
}
#`oraccopt . g2`
function cbd {
    g2="yes"
    if [[ $g2 == "yes" ]]; then
	# it's OK for there to be no glossary in cbd=dynamic
	if [ -r 01tmp/$l.glo ] ; then
	    echo "g2=yes, creating $ldir/$l.cbd via 01tmp/$l.glo ..."
	    l2-glomanager.plx -xml 01tmp/$l.glo -out $ldir/$l.cbd
	fi
    elif [ -r 01bld/$l.glo.norm ]; then
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
#	echo l2p2-g2x.plx -all -h $ldir/union.sig
	l2p2-g2x.plx -all -h $ldir/union.sig
    else
#	echo l2p2-g2x.plx -h $ldir/union.sig
	l2p2-g2x.plx -h $ldir/union.sig
    fi
    #    echo running xis $ldir $l
    if [ "$project" == "epsd2" ]; then
	echo running emesalify for epsd2
	emesalify.plx
	echo emesalify done
    fi
    xis $ldir $l
    if [ -s $ldir/$l.map ]; then
#	echo running l2p2-g2c.plx $l
	l2p2-g2c.plx $l
	export ORACC_HOME
#-verb
	l2-glomanager.plx -conf l2p2.xcf -cbdlang $l
    else
	echo Glossary $l is empty.  Exterminate.
    fi
}

rm -f 01bld/cancel
projtype=`oraccopt . type`
super=`oraccopt . cbd-super`
#echo projtype=$projtype
if [ "$projtype" == "superglo" ]; then
    for aa in `oraccopt . cbd-super-list` ; do
	# new: cbd-super-list could be qpn/sux in which case drop /sux
	aaa=`/bin/echo -n $aa | perl -p -e 's#/.*$##'`
	if [ "$aaa" == "qpn" ]; then
	    aa=`l2p2-langs.plx`
	fi
	for l in $aa; do
	    ldir=01bld/$l
	    mkdir -p $ldir
	    cbd $l $ldir
	    if [ -e 01bld/cancel ]; then
		echo REBUILD CANCELLED
		exit 1
	    fi
	    echo l2p2.sh: processing sigs for superglo $a
	    rm -f $ldir/union.sig
	    [ -r 01bld/project.sig ] && l2p2-sig-slicer.plx -lang $l
	    l2-sig-union.plx -super $super -proj $project -lang $l $ldir/$l.sig >$ldir/union.sig
	    g2x $ldir $l
	done
    done
else
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

	# Rebuild union.sig
	rm -f $ldir/union.sig
	[ -r 01bld/project.sig ] && l2p2-sig-slicer.plx -lang $l
	[ -r 01bld/from-glos.sig ] && l2p2-sig-slicer.plx -lang $l -name glossary -sigs 01bld/from-glos.sig
	if [ -r $ldir/glossary.sig ] && [ -r $ldir/$l.sig ]; then
	    l2-sig-union.plx $ldir/$l.sig $ldir/glossary.sig >$ldir/union.sig
	elif [ -r $ldir/glossary.sig ]; then
	    l2-sig-union.plx $ldir/glossary.sig >$ldir/union.sig
	else
	    l2-sig-union.plx $ldir/$l.sig >$ldir/union.sig
	fi

	# Now we can build the XML version of the glossary
	if [ -s $ldir/union.sig ]; then
	    g2x $ldir $l
	else
	    (cd $ldir; ln -sf $l.cbd $l.g2c; ln -sf $ORACC/lib/data/dummy.xis periods.xis)
	    l2-glomanager.plx -conf l2p2.xcf -cbdlang $l
	    l2p2-letters.plx $l
	fi
    done
fi
#else
#    l2p2-union.plx
#    for a in `ls 01bld/*/union.sig` ; do
#	ldir=`dirname $a`
#	l=`basename $ldir`
#	g2x $ldir $l
#    done
#fi

if [ -r 01bld/pleiades.tab ]; then
    cp 01bld/pleiades.tab 02pub
fi

if [ -r 01bld/sux/articles.xml ]; then
    xsltproc $ORACC_BUILDS/lib/scripts/l2p2-oxtab.xsl 01bld/sux/articles.xml \
	     >01bld/sux/oidxid.tab
fi

## This is a temporary hack to do sense and base sorting in
## epsd2--this will almost certainly be applied to all projects soon

l2p2-new.sh $project

#usages=`oraccopt . cbd-usages`
#if [ "$usages" = "yes" ]; then
#    if [ "$project" = "epsd2" ]; then
#need to port this over to new build organization
#	epsd2-usages.sh
#    else
#	echo l2p2.sh: "cbd-usages only works with epsd2 at the moment--tell Steve to fix this!"
#    fi
#fi
