#!/bin/sh

cd ~

project=$1

if [ "$project" == "" ]; then
    echo "$0: must give project name on command line"
    exit 1
fi

if [[ ORACC_BUILDS == "" ]]; then
    echo "$0: ORACC_BUILDS not set. Stop."
    exit 1
fi

logdir=$ORACC_BUILDS/log/$project
mkdir -p $logdir
logfile=$logdir/stash-install.log

projfile=`/bin/echo -n $project | tr / -`
tarball=stash/srv/$projfile.tar.xz

if [ -r $tarball ]; then
    echo "Installing $tarball with logging to $logfile"

    exec 1>$logfile
    exec 2>&1

    tar -C $ORACC_BUILDS -Jxf $tarball
    cd $ORACC_BUILDS/$project
# why this mkdir ?
#    mkdir -p $project
    mkdir -p tmp/$project
    rm -f 01bld 01tmp 02pub 02www 02xml
    ln -sf $ORACC_BUILDS/bld/$project 01bld
    ln -sf $ORACC_BUILDS/tmp/$project 01tmp
    ln -sf $ORACC_BUILDS/pub/$project 02pub
    ln -sf $ORACC_BUILDS/www/$project 02www
    ln -sf $ORACC_BUILDS/xml/$project 02xml
    exit 0
else
    echo "stash-install.sh: failed to find $project tarball $tarball"
    exit 1
fi
