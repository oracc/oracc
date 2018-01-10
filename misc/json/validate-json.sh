#!/bin/sh
project=`oraccopt`
echo Validating project json for $project
(cd 01bld/json ;
 jlist=$ORACC_BUILDS/$project/01tmp/json.list ;
 find . -name '*.json' | sort >$jlist ;
 for a in `cat $jlist` ; do
     echo Validating $a
     json-licensify.plx <$a | jq . >j
     mv j $a
 done
 )
echo ''
