#!/bin/sh
shopt -s nullglob
fulldir=`pwd`
docdir=`echo $fulldir | perl -p -e 's#^.*?doc#www#'`
rm -fr $1/$docdir
mkdir -p $1/$docdir
cp html/* $1/$docdir
rnc=`find . -name '*.rnc'`
if [ "$rnc" != "" ]; then
    mkdir -p $1/lib/schemas
    cp -f *.rnc $1/lib/schemas
fi
