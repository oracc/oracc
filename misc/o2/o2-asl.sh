#!/bin/sh

slname=`oraccopt . asl-signlist`

if [[ "$slname" == "" ]]; then
    slname=project.asl
fi
if [ -r 00lib/$slname ]; then
    mkdir -p 02pub/sl
    sx -s 00lib/$slname >02pub/sl/sl.tsv
    if [ $? != 0 ]; then
       	exit
    fi
    slix -v
    sx -x 00lib/$slname >02xml/sl.xml
    chmod -R o+r 02pub/sl 02xml/sl.xml
fi
