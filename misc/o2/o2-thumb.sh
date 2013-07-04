#!/bin/sh
img=`oraccopt . image`
ext=`echo ${img##*.}`
if [ "$ext" = "png" ]; then
    path=`find 00web/images 00web/esp/site-content/00web/images -name $img | head -1`
    proj=`oraccopt`
    projbase=`basename $proj`
    mkdir -p 02www/images
    echo cp $path 02www/images/$projbase-thumb.png
    cp $path 02www/images/$projbase-thumb.png
# this is mac only, but right now the main build machine is a mac
    if [ -x `which sips` ]; then
	sips -Z 66 02www/images/$projbase-thumb.png >/dev/null 2>&1
    fi
    chmod o+r 02www/images/$projbase-thumb.png
    exit 0
else
    echo o2-thumb.sh: project image must be in .png format \(found $img\)
    exit 1
fi
