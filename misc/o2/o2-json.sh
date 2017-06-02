#!/bin/sh
JSON=`oraccopt . json`

if [ "$JSON" == "no" ]; then
    echo JSON=no
    exit 0
fi

shopt -s nullglob
project=`oraccopt`
jsondir=01bld/json
rm -fr $jsondir ; mkdir $jsondir

jsonlog=01tmp/json.log

echo "o2-json.sh: generating JSON for $project"

mv 01bld/catalogue.json 01bld/cat.geojson $jsondir

echo "o2-json.sh: metadata ..."
metadata-json.sh

echo "o2-json.sh: corpus ..."
corpus-json.plx >$jsonlog 2>&1

echo "o2-json.sh: indexes ..."
index-json.sh >>$jsonlog 2>&1

echo "o2-json.sh: validating and adding licensing ..."
validate-json.sh >>$jsonlog 2>&1

errors-json.plx
if [ -r 01tmp/json-error.log ];
then
    echo "o2-json.sh: internal errors detected in JSON processing; please tell Steve"
else
    echo o2-json.sh: JSON created and validated without errors
fi

echo "o2-json.sh: zipping json ..."
zip=`zip-json.sh`

ORACCJSON=$ORACC_BUILDS/www/json
mkdir -p $ORACCJSON
mv 01bld/json/$zip $ORACCJSON
chmod o+r $ORACCJSON/$zip

#rm -fr 01bld/json

echo "o2-json.sh: $zip created and public"
