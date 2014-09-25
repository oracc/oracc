#!/bin/sh
project=`proj-from-conf.sh`
if [ -e $ORACC/bin/$project-rebuild.sh ]; then
    exec $ORACC/bin/$project-rebuild.sh
    exit 1
fi
type=`oraccopt . type`
if [ "$type" == "" ]; then
    type=corpus
fi
if [ "$type" == "catalogue" ]; then
    type=catalog
fi
exe=${ORACC}/bin/o2-$type.sh
if [ -x $exe ]; then
    $exe $*
# allow $exe to set exit code
else
    echo o2-rebuild.sh: cannot run $exe
    exit 1
fi
