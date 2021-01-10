#!/bin/sh -e

# Invoke with -u for oracc update; 20180916: -u is now deprecated

# All of the different project types that may have glossaries must
# have produced a list of sigs from their respective corpus before
# this script is called.  For simple corpora this is done directly
# from their XTF files.  For umbrella corpus, superglo and megaglo
# types, different approaches are used to create from-prx-glo.sig.

projtype=`oraccopt . type`

exec >01log/l2p1.log ; exec 2>&1

if [ "$projtype" == "superglo" ]; then
    true;
else
    o2-cbdpp.sh
fi

# This may have been done already as part of the automatic
# update that is part of o2-prepare.sh, but it checks to see if the
# lem rebuild is necessary so there's not much extra work involved
#l2p1-from-glos.sh
l2p1-project-sig.plx

l2p1-simple-from-project.sh

if [ -r .nolemmdata ]; then
    rm .nolemmdata
else
    # if we have project.sig use it for lemm data, otherwise try
    # to reuse stats from previous lemm data with -u option
    if [ -s 01bld/project.sig ]; then
	l2p1-lemm-data.plx
    else
	l2p1-lemm-data.plx -u
    fi
fi
