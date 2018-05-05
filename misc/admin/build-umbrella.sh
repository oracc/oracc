#!/bin/sh

function fail {
    echo "$*. Stop."
    exit 1
}

if [ ! -r 00lib/config.xml ]; then
    fail "no 00lib/config.xml, must run in a project directory"
else
    project=`proj-from-conf.sh`
fi
if [ ! -r 00lib/umbrella.lst ]; then
    fail "no 00lib/umbrella.lst, must run in an umbrella project"
fi
projbuild="00bin/${project}-build-all.sh"
if [ -r $projbuild ]; then
    $projbuild
else
    subs=`cat 00bin/umbrella.lst`
    oracc update
    for a in $subs ; do (cd $a ; oracc resources ; oracc build) ; done
    oracc resources
    oracc build clean
fi
