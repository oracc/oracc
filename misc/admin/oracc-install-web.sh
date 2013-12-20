#!/bin/sh
srcdir=/tmp/oracc-runtimes
project=$1
tarball=$srcdir/web-$project.tar.gz
if [ -r $tarball ]; then
    tar -C $ORACC -zxf $tarball
    echo oracc install: installed runtime version of $project
    exit 0
else
    echo oracc-install-server.sh: no such file $tarball
    exit 1
fi
