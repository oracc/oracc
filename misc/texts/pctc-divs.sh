#!/bin/sh
shopt -s nullglob
cd 01tmp/xis
for a in o* ; do
    (cd $a ;
     tis=( *_*.tis )
     if [[ "$tis" == "" ]]; then
	 echo "pctc-divs.sh: no .tis files for OID $a"
     else
	 for t in *_*.tis ; do
	     d=`basename $t tis`div
	     #	 echo processing $t in $a with output to $d
	     head -5 <$t | wm -l -p pctc | ce_xtf -3 -p pctc -l | xis-xml-html.sh pctc >$d
	 done
     fi
    )
done
