#!/bin/sh
clouddir=$ORACC/Dropbox/oracc-builds
project=$1
if [ -r $clouddir/$project-runtime.tar.gz ]; then
    tar -C $ORACC -zxf $project-runtime.tar.gz
    echo oracc install: installed runtime version of $project
    exit 0
else
    echo oracc-install-server.sh: no such file $clouddir/$project-runtime.tar.gz.
    exit 1
fi
