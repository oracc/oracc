#!/bin/sh
servehost=`oraccopt . serve-host`
if [ "$servehost" == "" ]; then
    true;
else
    cd $ORACC_BUILDS/www/srv
    if [ -r $1 ]; then
	pid=$$
	mkdir $pid
	cd $pid
	tar zxf ../$1
	cd www
	for a in `grep -r -l http://localhost *` ; do
	    perl -pi -e "s/localhost/$servehost/g" $a
	done
    else
	echo "serve-host.sh: no such tarball $1. Stop."
	exit 1
    fi
fi
