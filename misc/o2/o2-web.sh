#!/bin/sh
[ -r 01bld/cancel ] && exit 1
[ -r .nobuild ] && exit 1
shopt -s nullglob
webdir=01bld/www ; rm -fr $webdir
# N.B.: can't create empty $webdir/cbd like this because it causes 
# oracc build portal to empty 02www/cbd
mkdir -p $webdir/t $webdir/css
cp -f 00web/projcss.css $webdir/css
cp 02xml/config.xml $webdir/config.xml
cp 00lib/thumb.png $webdir/thumb.png
if [ -r 00lib/project.sty ]; then
    cp -p 00lib/project.sty $webdir/
fi
p3-colours.plx $webdir
echo o2-web.sh calling o2-portal.sh
o2-portal.sh
