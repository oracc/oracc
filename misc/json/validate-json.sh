#!/bin/sh
(cd 01bld/json ; 
 for a in `find . -name '*.json'` ; do
     echo $a
     json-licensify.plx <$a | jq . >j
     mv j $a
 done
 )
