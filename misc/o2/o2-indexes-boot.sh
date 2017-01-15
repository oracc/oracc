#!/bin/sh
project=`oraccopt`
if [ -s 01bld/cdlicat.xmd ]; then
    cat 01bld/cdlicat.xmd | secatx -s -p $project
fi
if [ -s 01bld/lists/xtfindex.lst ]; then
    setxtx -p $project <01bld/lists/xtfindex.lst
    setrax -p $project <01bld/lists/xtfindex.lst
fi
if [ -s 01bld/lists/xtfindex.lst ]; then
    selemx -p $project <01bld/lists/lemindex.lst
fi
for a in 02pub/cbd/* ; do 
    lang=`basename $a`
    secbdx -p $project -l $lang 01bld/$lang/articles.xml
done
