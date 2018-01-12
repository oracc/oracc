#!/bin/sh
portals=portalpages
tmp=01tmp/portalpages
rm -fr $tmp
mkdir -p $tmp
cd $tmp
for a in $ORACC_BUILDS/www/json/*.zip ; do
    unzip -qq -j -o $a '*-portal.json' 2>/dev/null
done
cd ..
zip -ur $ORACC_BUILDS/www/json/neo.zip $portals
