#!/bin/sh
project=`oraccopt`
if [ "$project" = "" ]; then
    echo esp-init.sh: must run from a project directory
    exit 1
fi
cwd=`pwd` ; cd 00web
cp -R $ORACC/src/misc/esp/dist/esp .
if [ -d 00web/esp/site-content/00web ]; then
    if [ -d 00web/esp/site-content/00web/images ]; then
	cp -a images/* 00web/esp/site-content/00web/images
    fi
    echo <<EOF 
Images have been moved to 00web/esp/site-content/00web/images.
You may need to review the contents of 00web to see what else should
be moved to 00web/esp/site-content/00web.

You can now create new ESP pages in 00web/esp/site-content by
following the instructions in 
http://oracc.museum.upenn.edu/doc/manager/portals/esp.
EOF
fi
