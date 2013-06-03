#!/bin/sh
project=$1
if [ "$project" == "" ]; then
    echo usage: oracc install [PROJECT]
    exit 1
fi
mode=$ORACC_MODE
if [ -x $ORACC/bin/oracc-install-$mode.sh ]; then
    oracc-install$mode.sh $project
else
    echo oracc-install.sh: no installation procedure for mode=$mode
    exit 1
fi
