#!/bin/sh

## Paths for testing serve-install.sh
ORACC=/Users/stinney/test-serve-install
ORACC_BUILDS=/Users/stinney/test-serve-install
ORACC_HOST=dummy

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

mkdir -p $ORACC_BUILDS/srv
cd $ORACC_BUILDS/srv

tarball=$project.tar.gz
httpurl="http://$buildserver/srv/$tarball"
echo Retrieving $httpurl ...
curl -O $httpurl
if [ -s $tarball ]; then
    echo "Installing $tarball"
    cd $ORACC_BUILDS
    tar zxf srv/$tarball
    mkdir -p $project
    cd $project
    rm -f 01bld 02pub 02www 02xml
    ln -sf $ORACC/bld/$project 01bld
    ln -sf $ORACC/pub/$project 02pub
    ln -sf $ORACC/www/$project 02www
    ln -sf $ORACC/xml/$project 02xml
    cd $ORACC_BUILDS/$project
    o2-indexes-boot.sh
    est-project-web.sh
    cd 02www
    for a in `grep -r -l http://$buildserver *` ; do
	perl -pi -e "s/$buildserver/$ORACC_HOST/g" $a
    done
    touch $ORACC_BUILDS/$project/installstamp
    exit 0
else
    echo "serve-install.sh: must give valid tarball on command line"
    exit 1
fi
