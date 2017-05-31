#!/bin/sh
project=`oraccopt`
cd 01bld/json
touch manifest.json
find . -name '*.json' | sed 's#./##' | manifest-json.plx $project >manifest.json 
