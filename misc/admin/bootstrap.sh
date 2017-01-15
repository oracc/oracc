#!/bin/sh
project=$1
tarball=$2
if [ "$project" == "" ]; then
    echo "bootstrap.sh: must give project name on command line"
    exit 1
fi
if [ ${ORACC_BUILDS} = `pwd` ]; then
    echo installing $project;
else
    echo "bootstrap.sh: must be in Oracc builds ("`pwd`")"
    exit 1
fi
if [ -s $tarball ]; then
    tar zxf $tarball
    mkdir -p $project
    cd $project
    rm -f 01bld 02pub 02www 02xml
    ln -sf $ORACC_BUILDS/bld/$project 01bld
    ln -sf $ORACC_BUILDS/pub/$project 02pub
    ln -sf $ORACC_BUILDS/www/$project 02www
    ln -sf $ORACC_BUILDS/xml/$project 02xml
    o2-indexes-boot.sh
    est-project-web.sh
else
    echo "bootstrap.sh: must give valid tarball on command line"
    exit 1
fi
