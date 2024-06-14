#!/bin/sh
#o2-json.sh
pxreset.sh `oraccopt`
if [ -r 01bld/cat.geojson ]; then
    cp 01bld/cat.geojson 02www && chmod go+r 02www/cat.geojson
fi
oraccopt >01bld/buildstamp
touch $ORACC_BUILDS/snc/`oraccopt | tr / -`
