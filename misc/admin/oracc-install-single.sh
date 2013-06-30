#!/bin/sh
. $ORACC/bin/oracc-install-funcs.sh
project=$1
projdir=$ORACC_HOME/$project
projfile=`echo $project | tr / -`

echo dir=$projdir

if [ ! -d $projdir ]; then
    if [ -r $projfile-runtime.tar.gz ]; then
	tar -C $ORACC -zxf $projfile-runtime.tar.gz
	echo oracc install: installed runtime version of $project
	exit 0
    else
	if [ -r $projfile-00data.tar.gz ]; then
	    tar zxf $projfile-00data.tar.gz
	else
	    if [ -r 00data/$projfile-00data.tar.gz ]; then
		tar zxf 00data/$projfile-00data.tar.gz
	    else
		echo oracc-install.sh: no directory $projdir and no file $projfile-00data.tar.gz or $projfile-runtime.tar.gz
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
