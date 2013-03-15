#!/bin/sh
fullproj=`oraccopt`
project=`basename $fullproj`
if [ "$project" == "" ]; then
    echo o2-catf.sh: must be run from a project directory
    exit
fi
catf=`oraccopt . catf-ok`
if [ "$catf" = "yes" ]; then
    ox -I01bld/atfsources.lst | catf-manager.plx -s 2>01tmp/catf-manager.log >02pub/$project.catf
    echo o2-catf.sh: created 02pub/$project.catf
else
    echo o2-catf.sh: CATF output is not enabled for project '$project'
fi
