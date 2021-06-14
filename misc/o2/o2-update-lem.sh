#!/bin/sh
pwd
mkdir -p 01log
project=`proj-from-conf.sh`
if [ -e $ORACC/bin/$project-update.sh ]; then
    exec $ORACC/bin/$project-update.sh
    exit 1
else
    o2-cnf.sh
    l2p1.sh -u
    for a in `cat 01log/glo.err` ; do
	cat $a
    done
fi
