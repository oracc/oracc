#!/bin/sh
if [ "$1" = "" ]; then
    echo subproject.sh: must give project name as argument
    exit 1
fi
if [ ! -e 00lib/config.xml ]; then
    echo subproject.sh: this program must be run from an existing project
    exit 1
fi
proj=$1
if [ ! "$proj" != "${proj/*}" ]; then
    echo subproject.sh: no / allowed in project
fi
if [ ! -d $proj ]; then
    mkdir -p $proj
fi
parent=`oraccopt`
fullproj=$parent/$proj
cd $proj
umask 0026
for a in any atf bak bib bin cat lib map tei web xml ; do \
    mkdir -p 00$a ; \
done
mkdir -p 00lib/lists 00web/images
echo `pwd`
subprojdirs.sh $fullproj
