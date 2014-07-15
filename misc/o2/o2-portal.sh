#!/bin/sh
webdir=01bld/www
if [ -d 00web/00config ]; then
    o2-xml.sh
    o2-weblive.sh
    echo calling esp2.sh `oraccopt` ...
    esp2.sh `oraccopt`
    echo calling esp2-live.sh `oraccopt` force ...
    esp2-live.sh `oraccopt` force
elif [ -d 00web/esp ]; then
    echo o2-portal.sh: esp version 1.0 is no longer supported.  Please contact your liaison to upgrade.
    exit 1
#    oracc esp
#    echo You now need to call: oracc esp live to make the rebuilt portal live online
elif [ -e 00web/index.html ] || web-default-index.plx $webdir; then
    mkdir -p $webdir/images
    cp -fpR 00web/* $webdir ; rm -f $webdir/*~
    cp -fp 00lib/thumb.png $webdir
    o2-weblive.sh
else
    echo o2-portal.sh: no portal to rebuild and no index.html or way of building same. Stop.
fi
est-project.sh
