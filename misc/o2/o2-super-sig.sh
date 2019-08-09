#!/bin/sh
# For any project in 00lib/supersig.lst make sure there is a link to its sigs
superlang=$1
if [ "$superlang" == "" ]; then
    echo "$0: must give name of super-lang, e.g., sux"
    exit 1
fi
if [ -r 00lib/supersig.lst ]; then
    for a in `grep -v '^#' 00lib/supersig.lst`; do
	sigproj=`/bin/echo -n $a | cut -d: -f1`
	if [[ $a = *":"* ]]; then
	    siglang=`/bin/echo -n $a | cut -d: -f2`
	else
	    siglang=""
	fi
	if [ -r $ORACC_BUILDS/$sigproj/01bld/from-xtf-glo.sig ]; then
	    sig=$ORACC_BUILDS/$sigproj/01bld/from-xtf-glo.sig
	else
	    sig=$ORACC_BUILDS/$sigproj/01bld/from-prx-glo.sig
	fi
	sig01=`/bin/echo -n $sigproj | tr / -`
	sig01="01sig/$sig01.sig"
	proj=`oraccopt`
	echo Importing sigs from $sig into $sig01
	if [ "$siglang" == "" ]; then
#	    echo "l2p2-sig-slicer.plx -proj $proj -lang $superlang -stdout -sig $sig \>$sig01"
	    l2p2-sig-slicer.plx -proj $proj -lang $superlang -stdout -sig $sig >$sig01
	else
#	    echo "l2p2-sig-slicer.plx  -proj $proj -lang $siglang -superlang $superlang -exact -stdout -sig $sig \>$sig01"
	    l2p2-sig-slicer.plx -proj $proj -lang $siglang -superlang $superlang -exact -stdout -sig $sig >$sig01
	fi
    done
else
    echo "$0: no 00lib/supersig.lst, nothing to do."
fi
