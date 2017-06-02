#!/bin/sh
project=`oraccopt`
echo Validating project json for $project
(cd 01bld/json ; 
 for a in `find . -name '*.json'` ; do
     echo Validating $a
     json-licensify.plx <$a | jq . >j
     mv j $a
 done
 )
echo ''
