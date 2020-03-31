#!/bin/sh
PROG=oracc-install-runtime.sh
srcdir=/tmp/oracc-runtimes
if [ "$1" == "" ]; then
    echo $PROG: must give project on command line. Stop.
    exit 1
fi
project=`echo $1 | tr / -`;
tarball=$srcdir/web-$project.tar.xz
if [ -r $tarball ]; then
    tar -C $ORACC -Jxf $tarball
    echo $PROG: installed runtime version of $project
    exit 0
else
    echo $PROG: no such file $tarball
    exit 1
fi
