#!/bin/sh
shopt -s nullglob
weblistdir=$1
if [[ $weblistdir == "" ]]; then
    echo "weblists.sh: must give web list dir on command line. Stop."
    exit 1
fi
mkdir -p $weblistdir
for a in 00lib/lists/* ; do 
    tr '\r' '\n' <$a | tr -s '\n' >$weblistdir/`basename $a`
done
