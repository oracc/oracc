#!/bin/sh
tmp=`pwd`/01tmp/wid2err.tab
rm -f $tmp
for a in $* ; do (cd $ORACC_BUILDS/$a ; pwd ; wid2err <01bld/lists/have-xtf.lst >>$tmp) ; done

