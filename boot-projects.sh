#!/bin/sh
. ./oraccenv.sh
. ./oraccpaths.sh
#proj="xcat qcat ogsl cdli"
proj=xcat
for a in $proj ; do
    stash-install.sh $a
done
for a in $proj ; do
    serve-index.sh $a build-oracc.museum.upenn.edu
done
agg-project-list.sh
ln -sf $ORACC_BUILDS/pub/cdli/catpnums.vec $ORACC_BUILDS/lib/data/
