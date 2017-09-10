#!/bin/sh
shopt -s nullglob
project=`oraccopt`
if [ -s 01bld/cdlicat.xmd ]; then
    cat 01bld/cdlicat.xmd | secatx -s -p $project
else
    if [ -s 01bld/cdlicat.xmd.gz ]; then
	zcat 01bld/cdlicat.xmd.gz | secatx -s -p $project
    fi
fi
if [ -s 01bld/lists/xtfindex.lst ]; then
    setxtx -p $project <01bld/lists/xtfindex.lst
    setrax -p $project <01bld/lists/xtfindex.lst
fi
if [ -s 01bld/lists/xtfindex.lst ]; then
    selemx -p $project <01bld/lists/lemindex.lst
fi
for a in 02pub/cbd/[a-z]* ; do 
    lang=`basename $a`
    secbdx -p $project -l $lang 01bld/$lang/articles.xml
done
