#!/bin/sh

function dirlinks {
    for a in bld tmp; do
	mkdir -p $ORACC/$a/$project
	ln -s $ORACC/$a/$project $ORACC_HOME/$project/01$a
    done
    for a in pub www xml; do
	mkdir -p $ORACC/$a/$project
	ln -s $ORACC/$a/$project $ORACC_HOME/$project/02$a	
    done
    rm -fr $ORACC/$project
    ln -sf $ORACC_HOME/$project $ORACC/$project
}

if [ "$ORACC_MODE" != "single" ]; then
    echo oracc install only works in single user mode for now. Stop.
    exit 1
fi

project=$1

if [ "$project" == "" ]; then
    echo usage: oracc install [PROJECT]
    exit 1
fi

projdir=$ORACC_HOME/$project

if [ ! -d $projdir ]; then
    if [ -r $project-runtime.tar.gz ]; then
	tar -C $ORACC -zxf $project-runtime.tar.gz
	echo oracc install: installed runtime version of $project
	exit 0
    else
	if [ -r $project-00data.tar.gz ]; then
	    tar zxf $project-00data.tar.gz
	else
	    echo oracc-install.sh: no directory $projdir and no file $project-00data.tar.gz or $project-runtime.tar.gz
	    exit 1
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
