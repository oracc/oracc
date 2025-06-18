#!/bin/sh
#o2-json.sh
ood.sh
pxp3p4.sh `oraccopt`
pxreset.sh `oraccopt`
oid-pub.sh
if [ -r 01bld/cat.geojson ]; then
    cp 01bld/cat.geojson 02www && chmod go+r 02www/cat.geojson
fi
oraccopt >01bld/buildstamp
touch $ORACC_BUILDS/snc/`oraccopt | tr / -`
