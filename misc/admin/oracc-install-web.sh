#!/bin/sh
srcdir=/tmp/oracc-runtimes
project=$1
tarball=$srcdir/web-$project.tar.xz
if [ -r $tarball ]; then
    tar -C $ORACC -Jxf $tarball
    oracc-index-runtime.sh
    echo oracc install: installed runtime version of $project
    exit 0
else
    echo oracc-install-server.sh: no such file $tarball
    exit 1
fi
