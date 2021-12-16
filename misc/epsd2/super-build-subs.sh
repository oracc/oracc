#!/bin/sh
biglog=`pwd`/super-build-subs.log
rm -f $biglog
date >$biglog
for a in `cat 00lib/superbuild.lst`; do
    proj=`/bin/echo -n $a | cut -d: -f1`
    echo $proj
    (cd $ORACC_BUILDS/$proj ; rm -f nohup.out ;
     if [ -r 00bin/do.sh ] ; then
	 00bin/do.sh
     elif [ -r ../00bin/do.sh ]; then
	  ../00bin/do.sh
     fi
     oracc build >nohup.out 2>&1 ;
     cat nohup.out >>$biglog)
done
date >>$biglog
