#!/bin/sh
project=$1
if [[ $project = "" ]]; then echo "list-subprojects.sh: must give project name"; fi
(cd $ORACC_BUILDS/xml/$project ; 
    find . -name config.xml \
	| xargs -R 1 -I '{}' dirname '{}' \
	| sed 's#^./##' | grep -v ^.$ )
