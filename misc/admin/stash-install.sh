#!/bin/sh

cd ~

project=$1

if [ "$project" == "" ]; then
    echo "stash-install.sh: must give project name on command line"
    exit 1
fi

projfile=`/bin/echo -n $project | tr / -`
tarball=stash/srv/$projfile.tar.xz

if [ -r $tarball ]; then
    echo "Installing $tarball"
    tar -C $ORACC_BUILDS -Jxf $tarball
    mkdir -p $ORACC/tmp/$project
    mkdir -p $project
    cd $project
    rm -f 01bld 01tmp 02pub 02www 02xml
    ln -sf $ORACC/bld/$project 01bld
    ln -sf $ORACC/tmp/$project 01tmp
    ln -sf $ORACC/pub/$project 02pub
    ln -sf $ORACC/www/$project 02www
    ln -sf $ORACC/xml/$project 02xml
    exit 0
else
    echo "stash-install.sh: failed to find $project tarball $tarball"
    exit 1
fi
