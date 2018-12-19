#!/bin/sh
fullproj=`oraccopt`
project=`basename $fullproj`
if [ "$project" == "" ]; then
    echo o2-catf.sh: must be run from a project directory
    exit
fi
catf=`oraccopt . catf-ok`
if [ "$catf" = "yes" ]; then
    fproj=`/bin/echo -n $fullproj | tr '/' '-'`
    ox -I01bld/atfsources.lst | catf-manager.plx -s 2>01tmp/catf-manager.log >02pub/$fproj.catf
    echo o2-catf.sh: created 02pub/$fproj.catf
    errcount=`grep -c ^xtf 01tmp/catf-manager.log`
    echo 01tmp/catf-manager.log error count = $errcount
    host=`hostname | cut -d. -f1`
    if [[ $host = "build-oracc" ]]; then
	echo Copying $fproj.catf to catf repo
	cp 02pub/$fproj.catf /home/oracc/git/catf
    else
	echo Using scp to send $fproj.catf to build-oracc catf repo
	sudo -u stinney scp 02pub/$fproj.catf stinney@build-oracc.museum.upenn.edu:/home/oracc/git/catf	
    fi
    echo Updating github repo github.com/oracc/catf
    if [[ $host = "build-oracc" ]]; then
	sudo -u stinney /home/stinney/git-oracc-catf.sh $fproj.catf
    else
	sudo -u stinney ssh build-oracc.museum.upenn.edu ./git-oracc-catf.sh $fproj.catf
    fi
else
    echo o2-catf.sh: CATF output is not enabled for project $fullproject
fi
