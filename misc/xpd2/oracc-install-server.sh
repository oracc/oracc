#!/bin/sh
clouddir=$ORACC/Dropbox/oracc-builds
project=$1
tarball=$clouddir/$project-runtime.tar.gz
if [ -r $clouddir/$project-runtime.tar.gz ]; then
    tar -C $ORACC -zxf $tarball
    echo oracc install: installed runtime version of $project
    exit 0
else
    echo oracc-install-server.sh: no such file $tarball
    exit 1
fi
