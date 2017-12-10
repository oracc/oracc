#!/bin/sh
g=test.glo
# v=entry

for a in t/*.glo ; do
    v=`basename $a .glo`
    perl ./cbdpp.plx -v $v $a >t/$v.log
done
