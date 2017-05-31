#!/bin/sh
project=`oraccopt`

xsltproc ${ORACC}/lib/scripts/config-json.xsl 02xml/config.xml >01bld/json/config.json
# this builds witnesses.json
linknorm-json.plx $project
formats-json.plx >01bld/json/formats.json

(cd $ORACC_BUILDS/$project/01bld/json ;
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

rm -f 01bld/json/{config,formats,witnesses}.json
