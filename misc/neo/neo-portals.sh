#!/bin/sh
rm -fr 01tmp/portaljson
mkdir -p 01tmp/portaljson
cd 01tmp/portaljson
for a in $ORACC_BUILDS/www/json/*.zip ; do
    unzip $a '*-portal.json'
done
zip $ORACC_BUILDS/www/json/portalpages *.json
