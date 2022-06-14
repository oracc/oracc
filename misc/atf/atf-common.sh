#!/bin/sh
p1=$1
p2=$2
if [ "$p2" == "" ]; then
    echo "$0: must give project1 and project2 on command line"
    exit 1
fi
atfgrabpq.plx $ORACC_BUILDS/$p1/00atf/*.atf >p1.lst
atfgrabpq.plx $ORACC_BUILDS/$p2/00atf/*.atf >p2.lst
echo "$0: the following texts are in $p1 and $p2:"
atflists.plx -u p1.lst '&' p2.lst
