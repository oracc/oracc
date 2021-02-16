#!/bin/sh
fullproj=`oraccopt`
project=`basename $fullproj`
if [ "$project" == "" ]; then
    echo o2-catf.sh: must be run from a project directory
    exit
fi
#catf=`oraccopt . catf-ok`
#if [ "$catf" = "yes" ]; then
    fproj=`/bin/echo -n $fullproj | tr '/' '-'`
    ox -I01bld/atfsources.lst | catf-manager.plx -s 2>01tmp/catf-manager.log >02www/project.catf
    chown root:oracc 02www/project.catf
    chmod +r 02www/project.catf
    echo o2-catf.sh: you can now retrieve /$fullproj/project.catf
#else
#    echo o2-catf.sh: CATF output is not enabled for project $fullproject
#fi
