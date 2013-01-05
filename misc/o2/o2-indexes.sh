#!/bin/sh
project=`oraccopt`
gunzip -c 01bld/cdlicat.xmd.gz | secatx -s -p $project
setxtx -p $project <01bld/lists/xtfindex.lst
setrax -p $project <01bld/lists/xtfindex.lst
selemx -p $project <01bld/lists/lemindex.lst
for a in 02pub/cbd/* ; do 
    lang=`basename $a`
    secbdx -p $project -l $lang 01bld/$lang/articles.xml
done
dbi2est.plx
