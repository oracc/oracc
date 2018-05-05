#!/bin/sh
cd /home
for a in * ; do
    if [ $a == "cdli" -o $a == "epsd2" ]; then
	echo skipping $a
    else
	if [ -r "$a/00lib/config.xml" ]; then
	    echo backing up $a
	fi
    fi
done

