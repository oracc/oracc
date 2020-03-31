#!/bin/sh
. $ORACC/bin/oracc-install-funcs.sh
project=$1
projdir=$ORACC_HOME/$project
projfile=`echo $project | tr / -`

echo dir=$projdir

if [ ! -d $projdir ]; then
    if [ -r $projfile-runtime.tar.xz ]; then
	tar -C $ORACC -Jxf $projfile-runtime.tar.xz
	echo oracc install: installed runtime version of $project
	exit 0
    else
	if [ -r $projfile-00data.tar.xz ]; then
	    tar Jxf $projfile-00data.tar.xz
	else
	    if [ -r 00data/$projfile-00data.tar.xz ]; then
		tar Jxf 00data/$projfile-00data.tar.xz
	    else
		echo oracc-install.sh: no directory $projdir and no file $projfile-00data.tar.xz or $projfile-runtime.tar.xz
		exit 1
	    fi
	fi
    fi
fi

if [ -d $projdir ]; then
    if [ -r $projdir/00lib/config.xml ]; then
	dirlinks
    else
	echo oracc install only handles completing installation for now. Stop.
	exit 1
    fi
else
    echo oracc install doesn\'t handle auto-install yet. Stop.
    exit 1
fi
