#!/bin/sh
fulldir=`pwd`
docdir=`echo $fulldir | perl -p -e 's#^.*?doc#www/doc#'`
rm -fr $1/$docdir
mkdir -p $1/$docdir
cp html/* $1/$docdir
