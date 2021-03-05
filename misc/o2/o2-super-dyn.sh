#!/bin/sh
# For any project in 00lib/superdyn.lst update the map file
# dynamically
superglo=$1
if [ "$superglo" == "" ]; then
    echo "$0: must give name of super-glossary, e.g., 01tmp/sux.glo"
    exit 1
fi
if [ -r 00lib/superdyn.lst ]; then
    ext=0
    for a in `grep -v '^#' 00lib/superdyn.lst`; do
	echo Mapping $a into $superglo
	cbd=
	if [[ "$superglo" == "01tmp/qpn.glo" && "$a" == "epsd2/emesal:qpn" ]]; then
	    cbd="-glo $ORACC_BUILDS/epsd2/emesal/01tmp/qpn.glo"
	    echo cbd=$cbd
	fi
	thisproj=`oraccopt`
	if [[ "$thisproj" == "epsd2/names" ]]; then
	    if [ -r $ORACC_BUILDS/$a/00lib/qpn.glo ]; then
		cat $ORACC_BUILDS/$a/00lib/qpn.glo | cbdqpnsplit.plx sux
		hyp=`/bin/echo $a | tr / -`
		mv split-lang.glo 00raw/$hyp.glo
		rm -f split-base.glo
		cbd="-glo 00raw/$hyp.glo"
		echo cbd=$cbd
	    fi
	fi
	ext=$((1+$ext))
	cbd-super-compare.plx -apply -dynamic -incr $ext -inplace -base $superglo $cbd -proj $a
	if [ -r '.supercancel' ]; then
	    echo "o2-super-dyn.sh: failed to map $a into $superglo (incr=$ext)"
	    exit 1
	fi
    done
    superlang=`basename $superglo .glo`
    for m in 01map/*.map ; do
	cbd-super-map-sigs.plx -lang $superlang -auto $m
    done
else
    echo "$0: no 00lib/superdyn.lst, nothing to do."
fi
