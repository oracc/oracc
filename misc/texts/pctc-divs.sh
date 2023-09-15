#!/bin/sh
cd 01tmp/xis
for a in o* ; do
    (cd $a ;
     for t in *_*.tis ; do
	 d=`basename $t tis`div
#	 echo processing $t in $a with output to $d
	 head -5 <$t | wm -l -p pctc | ce_xtf -3 -p pctc -l | xis-xml-html.sh pctc >$d
     done
    )
done
