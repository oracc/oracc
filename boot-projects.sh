#!/bin/sh
. ./oraccenv.sh
proj="xcat qcat ogsl cdli"
for a in $proj ; do
    serve-install.sh $a build-oracc.museum.upenn.edu
done
for a in $proj ; do
    serve-index.sh $a build-oracc.museum.upenn.edu
done
ln -sf $ORACC_BUILDS/pub/cdli/catpnums.vec $ORACC_BUILDS/lib/data/
