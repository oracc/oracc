#!/bin/sh
project=$1

# this builds witnesses.json
linknorm-json.plx $project
formats-json.plx >>01bld/formats.json

(cd $ORACC_BUILDS/$project/01bld ;
 echo '{' >metadata.json
 echo '	"type": "metadata",' >>metadata.json
 projprop="\"project\": \"$project\","
 echo '	'$projprop >>metadata.json
 cat config.json >>metadata.json
 for j in formats.json witnesses.json; do
     if [ -r $j ]; then
	 echo ',' >>metadata.json
	 cat $j >>metadata.json
     fi
 done
 echo >>metadata.json
 echo '}' >>metadata.json
 )
