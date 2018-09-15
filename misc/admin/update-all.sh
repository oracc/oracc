#!/bin/sh

function fail {
    echo "$*. Stop."
    exit 1
}

if [ ! -r 00lib/config.xml ]; then
    fail "no 00lib/config.xml"
else
    project=`proj-from-conf.sh`
fi
oracc update
subs=`list-all-subs.sh`
for a in $subs ; do (cd $a ; oracc update) ; done
