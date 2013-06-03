#!/bin/sh
proj=$1
if [ "$proj" == "" ]; then
    echo oracc-project-runtime.sh: must give project on command line
    exit 1
fi
if [ -w $ORACC_HOME/$proj/00any ]; then
    destdir=$ORACC_HOME/$proj/00any
else
    destdir=$ORACC/00any
fi
if [ ! -w $destdir ]; then
    echo oracc-project-runtime.sh: cannot write to $destdir. Stop
    exit 1
fi
fproj=`/bin/echo -n $proj | tr / -`
echo :$fproj:
cd $ORACC
tar --exclude www/$proj/estindex -zcf $destdir/$fproj-runtime.tar.gz \
    agg/projects/images/$proj.png \
    bld/$proj/[PQX][0-9][0-9][0-9] \
    pub/$proj \
    xml/$proj \
    www/$proj
