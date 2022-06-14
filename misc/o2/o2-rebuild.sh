#!/bin/sh

mkdir -p 01log

project=`proj-from-conf.sh`
hproj=`/bin/echo -n $project | tr / -`

echo o2-rebuild project = $project
echo o2-rebuild hproj = $hproj

if [ -e $ORACC/bin/$hproj-rebuild.sh ]; then
    echo o2-rebuild.sh redirecting to $hproj-rebuild.sh
    exec $ORACC/bin/$hproj-rebuild.sh
    exit 1
fi
type=`oraccopt . type`

if [ "$type" == "" ]; then
    type=corpus
fi

if [ "$type" == "catalogue" ]; then
    type=catalog
fi

if [[ $type == "corpus" ]]; then
    o2-update-lem.sh
fi

exe=${ORACC}/bin/o2-$type.sh
if [ -x $exe ]; then
    $exe $*
# allow $exe to set exit code
else
    echo o2-rebuild.sh: cannot run $exe
    exit 1
fi
