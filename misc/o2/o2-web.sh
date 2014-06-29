#!/bin/sh
[ -r 01bld/cancel ] && exit 1
shopt -s nullglob
webdir=01bld/www ; rm -fr $webdir
mkdir -p $webdir/t $webdir/cbd $webdir/css
cp 02xml/config.xml $webdir/config.xml
cp 00lib/thumb.png $webdir/thumb.png
if [ -r 00lib/project.sty ]; then
    cp -p 00lib/project.sty $webdir/
fi
p3-colours.plx $webdir
o2-portal.sh
