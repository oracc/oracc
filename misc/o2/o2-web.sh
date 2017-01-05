#!/bin/sh
[ -r 01bld/cancel ] && exit 1
shopt -s nullglob
webdir=01bld/www ; rm -fr $webdir
# N.B.: can't create empty $webdir/cbd like this because it causes 
# oracc build portal to empty 02www/cbd
mkdir -p $webdir/t $webdir/css
cp 02xml/config.xml $webdir/config.xml
cp 00lib/thumb.png $webdir/thumb.png
cp 01bld/metadata.json $webdir/metadata.json
if [ -r 00lib/project.sty ]; then
    cp -p 00lib/project.sty $webdir/
fi
p3-colours.plx $webdir
o2-portal.sh
