#!/bin/sh
ORACC=$1
srcdir=$2

shopt -s nullglob

function install_one_pm {
    packagedir=$ORACC/lib/`$srcdir/package_dir.sh $1`
    mkdir -p $packagedir && chmod go+rx $packagedir 
    pm=$packagedir/`basename $1` 
    rm -f $pm 
    echo installing $pm
    perl -C0 -pe "s#\@\@ORACC\@\@#$ORACC#g" <$1 >$pm
    chmod o+r $pm ; chmod -w $pm
}

pm=`ls *.pm`
if [ "$pm" != "" ]; then
    for each in *.pm ; do install_one_pm $each ; done
fi
