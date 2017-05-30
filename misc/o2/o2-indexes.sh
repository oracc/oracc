#!/bin/sh
shopt -s nullglob
project=`oraccopt`
#gunzip -c 01bld/cdlicat.xmd.gz | secatx -s -p $project
cat 01bld/cdlicat.xmd | secatx -s -p $project
setxtx -p $project <01bld/lists/xtfindex.lst
setrax -p $project <01bld/lists/xtfindex.lst
selemx -p $project <01bld/lists/lemindex.lst

dbi2est.plx
