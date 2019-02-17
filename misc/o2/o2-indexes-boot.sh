#!/bin/sh
shopt -s nullglob
project=`oraccopt`
echo o2-indexes-boot.sh: working on $project in `pwd`
if [ -s 01bld/cdlicat.xmd ]; then
    echo indexing catalogue
    cat 01bld/cdlicat.xmd | secatx -s -p $project
else
    if [ -s 01bld/cdlicat.xmd.gz ]; then
	echo indexing catalogue
	zcat 01bld/cdlicat.xmd.gz | secatx -s -p $project
    fi
fi
if [ -s 01bld/lists/xtfindex.lst ]; then
    echo indexing transliterations
    setxtx -p $project <01bld/lists/xtfindex.lst
    echo indexing translations
    setrax -p $project <01bld/lists/xtfindex.lst
fi
if [ -s 01bld/lists/xtfindex.lst ]; then
    echo indexing lemmatized texts
    selemx -p $project <01bld/lists/lemindex.lst
fi
if [ -s 02pub/sl/*.tsv ]; then
    sl-index 02pub/sl/*.tsv
fi
for a in 02pub/cbd/[a-z]* ; do 
    lang=`basename $a`
    if [ -f 01bld/$lang/articles.xml ]; then
	/bin/echo -n indexing $lang glossary
	secbdx -p $project -l $lang 01bld/$lang/articles.xml
	echo ", compiling instances database"
	lex-tis.sh
	xisdb 02pub/cbd/$lang/$lang.tis
    fi
done
echo o2-indexes-boot.sh: done.
