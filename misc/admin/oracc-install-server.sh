#!/bin/sh
clouddir=$ORACC/Dropbox/oracc-builds
project=$1
tarball=$clouddir/$project-runtime.tar.xz
if [ -r $clouddir/$project-runtime.tar.xz ]; then
    tar -C $ORACC -Jxf $tarball
    echo oracc install: installed runtime version of $project
    exit 0
else
    echo oracc-install-server.sh: no such file $tarball
    exit 1
fi
