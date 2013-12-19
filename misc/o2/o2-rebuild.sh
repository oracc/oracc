#!/bin/sh
o2-cnf.sh || exit 1
if [ -e 00bin/prebuild.sh ]; then
    00bin/prebuild.sh
fi
if [ -e 00bin/rebuild.sh ]; then
    exec 00bin/rebuild.sh
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
    o2-rebuild.sh: cannot run $exe
    exit 1
fi
if [ -e 00bin/postbuild.sh ]; then
    00bin/postbuild.sh
fi
