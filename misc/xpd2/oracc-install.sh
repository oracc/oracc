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
if [ -d $projdir ]; then
    if [ -d $projdir/.git ]; then
	dirlinks
    else
	echo oracc install only handles git install completion for now. Stop.
	exit 1
    fi
else
    echo oracc install doesn\'t handle auto-install yet. Stop.
    exit 1
fi
