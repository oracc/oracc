#!/bin/sh
#
# List signlist OIDs that are used in the signlist for the project.
#
p=$1
if [[ "$p" == "" ]]; then
    p=`oraccopt`
    if [[ "$p" == "" ]]; then
	echo "$0: either give project on command-line or run me in a project directory. Stop."
	exit 1
    fi
fi
grep ^o ${ORACC_BUILDS}/$p/02pub/sl/sl.tsv | cut -f1 | grep -v ';' | sort -u
