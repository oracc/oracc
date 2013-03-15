#!/bin/sh
shopt -s nullglob
webdir=$1
shift 1
if [ "$webdir" == "" ]; then
    echo l2p3.sh: must give webdir on command line
    exit 1
fi
rm -fr $webdir/cbd ; mkdir -p $webdir/cbd
g2c=`echo -n 01bld/*/*.g2c`
if [ "$g2c" != "" ]; then
    for g2c in `ls 01bld/*/*.g2c` ; do
	ldir=`dirname $g2c`
	l=`basename $ldir`
	echo producing web version of $l
	l2-glomanager.plx -webdir=$webdir -conf l2p3.xcf -cbdlang $l $*
    done
fi

