#!/bin/sh
project=`proj-from-conf.sh`
if [ -e $ORACC/bin/$project-update.sh ]; then
    exec $ORACC/bin/$project-update.sh
    exit 1
else
    o2-cnf.sh
    l2p1.sh
fi
