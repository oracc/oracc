#!/bin/sh
shopt -s nullglob
f=`find 01bld/ -name articles.xml`
if [ "$f" != "" ]; then
    for a in `(cd 01bld ; ls */articles.xml)` ; do 
	if [ "$a" != "." ]; then
	    echo `dirname $a` 
	fi
    done
fi
