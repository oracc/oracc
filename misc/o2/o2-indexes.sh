#!/bin/sh
project=`oraccopt`
#gunzip -c 01bld/cdlicat.xmd.gz | secatx -s -p $project
cat 01bld/cdlicat.xmd | secatx -s -p $project
setxtx -p $project <01bld/lists/xtfindex.lst
setrax -p $project <01bld/lists/xtfindex.lst
selemx -p $project <01bld/lists/lemindex.lst

sort -u -o 02pub/cat/mangle.tab 02pub/cat/mangle.tab
sort -u -o 02pub/txt/mangle.tab 02pub/txt/mangle.tab
sort -u -o 02pub/tra/mangle.tab 02pub/tra/mangle.tab
sort -u -o 02pub/lem/mangle.tab 02pub/lem/mangle.tab

for a in 02pub/cbd/* ; do 
    lang=`basename $a`
    secbdx -p $project -l $lang 01bld/$lang/articles.xml
    cp 01bld/$lang/letter_ids.tab $a
    sort -u -o $a/mangle.tab $a/mangle.tab
done
dbi2est.plx
