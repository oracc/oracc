#!/bin/sh
if [ ! -e 00lib/config.xml ]; then
    echo "version.sh: not in a project or subproject directory"
    exit 1;
fi
n=`oracc name`
p=`oraccopt`
v=`version.plx`
oracc subproject $v "$n $v"
oracc clone $p $p/$v
(cd $v ; oracc rebuild; oracc freeze)
