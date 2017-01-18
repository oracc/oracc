#!/bin/sh
project=$1
buildserver=$2
if [ "$project" == "" ]; then
    echo "serve-install.sh: must give project name on command line"
    exit 1
fi
if [ "$buildserver" == "" ]; then
    echo "serve-install.sh: must give buildserver on command line"
    exit 1
fi
tarball=$project.tar.gz
httpurl="http://$buildserver/srv/$tarball"
cd $ORACC_BUILDS/srv
curl -O $httpurl
if [ -s $tarball ]; then
    cd $ORACC_BUILDS
    tar zxf srv/$tarball
    mkdir -p $project
    cd $project
    rm -f 01bld 02pub 02www 02xml
    ln -sf $ORACC_BUILDS/bld/$project 01bld
    ln -sf $ORACC_BUILDS/pub/$project 02pub
    ln -sf $ORACC_BUILDS/www/$project 02www
    ln -sf $ORACC_BUILDS/xml/$project 02xml
    cd $ORACC_BUILDS
    o2-indexes-boot.sh
    est-project-web.sh
else
    echo "serve-install.sh: must give valid tarball on command line"
    exit 1
fi
