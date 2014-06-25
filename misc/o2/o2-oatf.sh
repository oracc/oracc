#!/bin/sh
fullproj=`oraccopt`
project=`basename $fullproj`
if [ "$project" == "" ]; then
    echo o2-aatf.sh: must be run from a project directory
    exit
fi
ox -I01bld/atfsources.lst | oatf-manager.plx -s 2>01tmp/oatf-manager.log >01bld/oatf.tab
echo o2-oatf.sh: created 01bld/oatf.tab
