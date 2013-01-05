#!/bin/sh
#
# Import a project for testing, remapping the project
# attributes in the XTF to a local project name.
locproj=$1
source=$2
if [ "$locproj" = "" ]; then
    echo usage: import.sh local-project-name source-project-name;
    exit 1;
fi
rm -fr $locproj
mkdir -p $locproj
cd $locproj
mkdir -p 01bld 01lib
for a in `find $ORACC/bld/$source/[a-z][a-z][a-z]* -type d` ; do ln -sf $a 01bld; done
