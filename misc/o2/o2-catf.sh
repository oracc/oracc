#!/bin/sh
fullproj=`oraccopt`
project=`basename $fullproj`
if [ "$project" == "" ]; then
    echo o2-catf.sh: must be run from a project directory
    exit
fi
ox -I01bld/atfsources.lst | catf-manager.plx -s 2>01tmp/catf-manager.log >02www/project.catf
chown root:oracc 02www/project.catf
chmod +r 02www/project.catf
echo '####'
echo
echo You can now retrieve http://$ORACC_HOST/$fullproj/project.catf
echo
echo '####'
