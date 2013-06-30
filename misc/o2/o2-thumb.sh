#!/bin/sh
img=`oraccopt . image`
ext=`echo ${img##*.}`
if [ "$ext" = "png" ]; then
    path=`find 00web -name $img`
    proj=`oraccopt`
    projbase=`basename $proj`
    mkdir -p 02www/images
    cp $path 02www/images/$projbase-thumb.png
    chmod o+r 02www/images/$projbase-thumb.png
    exit 0
else
    echo o2-thumb.sh: project image must be in .png format \(found $img\)
    exit 1
fi
