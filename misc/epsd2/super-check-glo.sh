#!/bin/sh
supercancel=`pwd`/.supercancel
rm -f $supercancel
for a in `cat 00lib/super*.lst`; do
    proj=`/bin/echo -n $a | cut -d: -f1`
    (cd $ORACC_BUILDS/$proj ;
     for g in 00lib/*.glo ;
     do
	 if [ "$g" = "00lib/*.glo" ]; then
	     true
	 else
	     # we don't use -check here because that way we get updated 01tmp/sux.glo as a side-effect
	     # of running this check, which means we can also rebuild the main epsd2 sux.glo cheaply before
	     # rebuilding everything, and fail out of the nightly build early if it doesn't work
	     echo "super-check.plx working in $proj" >&2 ; cbdpp.plx -anno $g
	     if [ -r 01bld/cancel ]; then
		 echo $proj/$g >>$supercancel
	     fi
	 fi
     done)
done
