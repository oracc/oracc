#!/bin/sh
shopt -s nullglob
project=`oraccopt`
#gunzip -c 01bld/cdlicat.xmd.gz | secatx -s -p $project
cat 01bld/cdlicat.xmd | secatx -s -p $project

if [[ "$project" == "neo" ]]; then
    echo Skipping txt/tra/lem indexing for project $project
else
    setxtx -p $project <01bld/lists/xtfindex.lst
    setrax -p $project <01bld/lists/xtfindex.lst
    selemx -p $project <01bld/lists/lemindex.lst
fi
if [[ "$project" == "cdli" ]]; then
    echo Skipping dbi2est processing for project $project
elif [[ "$project" == "epsd2" ]]; then
    echo Skipping dbi2est processing for project $project
elif [[ "$project" == "neo" ]]; then
    echo Skipping dbi2est processing for project $project
else
    dbi2est.plx
fi
