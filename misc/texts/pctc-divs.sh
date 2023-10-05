#!/bin/sh
shopt -s nullglob
allthresh=30
cd 01tmp/xis
for a in o* ; do
    (cd $a ;
     itot=`wc -l $a.tis | cut -d' ' -f1`
     tis=( *_*.tis )
     if [[ "$tis" == "" ]]; then
	 echo "pctc-divs.sh: no .tis files for OID $a"
     else
	 for t in *_*.tis ; do
	     d=`basename $t tis`div
	     #	 echo processing $t in $a with output to $d
	     #
	     # If the total number of instances is <= $allthresh we inline everything;
	     # Else we inline the first five of each period.
	     #
	     if [[ $itot -le $allthresh ]]; then
		 cat $t | wm -l -p pctc | ce_xtf -3 -p pctc -l | xis-xml-html.sh pctc >$d
	     else
		 head -5 <$t | wm -l -p pctc | ce_xtf -3 -p pctc -l | xis-xml-html.sh pctc >$d
	     fi
	 done
     fi
    )
done
