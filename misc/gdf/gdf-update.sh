#!/bin/sh
dataset=$1
project=`oraccopt`
if [ "$project" == "" ]; then
    echo gdf-update.sh: must be run from a project directory
    exit 1
fi
if [ "$dataset" == "" ]; then
    echo gdf-update.sh: must give name of dataset on command line
    exit 1
fi
if [ ! -r 03ood/$dataset/gdf.xml ]; then
    echo gdf-update.sh: no gdf data to install from 03ood/$dataset/gdf.xml
    exit 1
fi
cd 03ood/$dataset
xsltproc $ORACC/lib/scripts/gdf-index-html.xsl gdf.xml >index.html
if [ -r $dataset.xml ]; then
    xsltproc $ORACC/lib/scripts/gdf-ids.xsl $dataset.xml >$dataset.ids
    rm -fr estindex
    xsltproc -stringparam project $project $ORACC/lib/scripts/gdf-est.xsl $dataset.xml \
	| est-index-uris.plx estindex
fi
mkdir -p $ORACC/pub/$project/gdf/$dataset/cat
segdfx -p $project -g $dataset < $dataset.xml
chmod -R o+r *
