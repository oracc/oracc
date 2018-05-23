#!/bin/sh

# All of the different project types that may have glossaries must
# have produced a list of sigs from their respective corpus before
# this script is called.  For simple corpora this is done directly
# from their XTF files.  For umbrella corpus, superglo and megaglo
# types, different approaches are used to create from-prx-glo.sig.

function sortlemm {
    grep ^@fields $1 >tmp.fields
    grep -v ^@fields $1 | sort -t'	' -nrk3 >tmp.sort
    cat tmp.fields tmp.sort >$1
}

projtype=`oraccopt . type`

g2=`oraccopt . g2`

if [ "$g2" == "yes" ]; then
    if [ -d "00src" ]; then
	srcglo=`ls -1 00src/*.glo | grep -v '~' | grep glo`
    else
	srcglo=""
    fi
    for a in $srcglo ; do
	cbdpp.plx $a
    done
    libglo=`ls -1 00lib/*.glo | grep -v '~' | grep glo`
    for a in $libglo ; do
	globase=`basename $a`
	if [ ! -r "00src/$globase" ]; then
	    cbdpp.plx $a
	fi
    done
fi

# This may have been done already as part of the automatic
# update that is part of o2-prepare.sh, but it checks to see if the
# lem rebuild is necessary so there's not much extra work involved
l2p1-from-glos.sh
l2p1-project-sig.plx
l2p1-lemm-data.plx
#(cd 02pub ; 
# for a in lemm-*.new ; do
#     sig=`basename $a .new`.sig
#     mv $a $sig
# done
#)
if [ -s 01bld/sortinfo.tab ]; then
    l2p1-sig-invert.plx
fi
