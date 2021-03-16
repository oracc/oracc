#!/bin/sh
shopt -s nullglob
project=`oraccopt`
echo o2-indexes-boot.sh: working on $project in `pwd`

if [ -s 01bld/cdlicat.xmd ]; then
    echo indexing catalogue
    cat 01bld/cdlicat.xmd | secatx -s -p $project
    pqxrecurse.plx $project
    chmod -R o+r 02pub/cat
else
    if [ -s 01bld/cdlicat.xmd.gz ]; then
	echo indexing catalogue
	zcat 01bld/cdlicat.xmd.gz | secatx -s -p $project
	pqxrecurse.plx $project
	chmod -R o+r 02pub/cat
    fi
fi

if [ -s 01bld/lists/xtfindex.lst ]; then
    echo indexing transliterations
    if [ -r $ORACC_BUILDS/pub/epsd2/alias-dumb.txt ]; then
	cp -av $ORACC_BUILDS/pub/epsd2/alias-dumb.txt 02pub/txt/aliases
    fi
    setxtx -p $project <01bld/lists/xtfindex.lst
    chmod -R o+r 02pub/txt
    echo indexing translations
    setrax -p $project <01bld/lists/xtfindex.lst
    chmod -R o+r 02pub/tra
fi

if [ -s 01bld/lists/lemindex.lst ]; then
    echo indexing lemmatized texts
    if [ -r $ORACC_BUILDS/pub/epsd2/alias-dumb.txt ]; then
	cp -av $ORACC_BUILDS/pub/epsd2/alias-dumb.txt 02pub/lem/aliases
    fi
    selemx -p $project <01bld/lists/lemindex.lst
    chmod -R o+r 02pub/lem
fi

for a in 02pub/cbd/[a-z]* ; do 
    lang=`basename $a`
    if [ -f 01bld/$lang/articles.xml ]; then
	/bin/echo -n indexing $lang glossary
	if [[ "$lang" == "sux"* ]]; then
	    if [ -r $ORACC_BUILDS/pub/epsd2/alias-dumb.txt ]; then
		cp -av $ORACC_BUILDS/pub/epsd2/alias-dumb.txt 02pub/cbd/$lang/aliases
	    fi
	fi
	secbdx -p $project -l $lang 01bld/$lang/articles.xml
	echo "	(compiling $lang instances database)"
	lex-tis.sh
	xisdb 02pub/cbd/$lang/$lang.tis
	chmod -R o+r 02pub/cbd/$lang
    fi
done
hproj=`/bin/echo -n $project | tr / -`
if [ -r 02pub/sl/$hproj-db.tsv ]; then
    echo indexing signlist
    if [ "$project" == "ogsl" ]; then
	sl-index -boot
    else
	cat 02pub/sl/$hproj-db.tsv | sl-index $project
    fi
    chmod -R o+r 02pub/sl
fi

echo o2-indexes-boot.sh: done.
