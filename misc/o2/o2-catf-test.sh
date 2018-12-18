#!/bin/sh
fullproj=`oraccopt`
project=`basename $fullproj`
if [ "$project" == "" ]; then
    echo o2-catf.sh: must be run from a project directory
    exit
fi
catf=`oraccopt . catf-ok`
if [ "$catf" = "yes" ]; then
    echo o2-catf.sh: CATF output is properly enabled for project $project
else
    echo o2-catf.sh: CATF output is not enabled for project $project but testing anyway
fi
fproj=`/bin/echo -n $fullproj | tr '/' '-'`
ox -I01bld/atfsources.lst | catf-manager.plx -s 2>01tmp/catf-manager.log >02pub/$fproj.catf
echo o2-catf.sh: created 02pub/$fproj.catf
errcount=`grep -c ^xtf 01tmp/catf-manager.log`
echo 01tmp/catf-manager.log error count = $errcount

