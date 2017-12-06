#!/bin/sh
project=$1
if [[ $project = "" ]]; then echo "list-subprojects.sh: must give project name"; exit 1; fi
(cd $ORACC_BUILDS/xml ; 
    find $project -name config.xml \
	| xargs -R 1 -I '{}' dirname '{}' \
	| grep / | sort)
