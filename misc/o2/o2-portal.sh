#!/bin/sh
webdir=01bld/www
project=`oraccopt`

if [ ! -r 02xml/config.xml ]; then
    o2-cnf.sh
fi

if [ -d 00web/00config ]; then
    o2-xml.sh
    if [ -d 01bld/www ]; then
	o2-weblive.sh
    fi
    echo calling esp2.sh `oraccopt` ...
    esp2.sh $project
    if [[ "$project" == "cdli" ]]; then
	echo Skipping est processing for project CDLI
    elif [[ "$project" == "epsd2" ]]; then
	echo Skipping est processing for project $project
    elif [[ "$project" == "neo" ]]; then
	echo Skipping est processing for project $project
    else
	est-project.sh
    fi
    echo calling esp2-live.sh `oraccopt` force ...
    esp2-live.sh `oraccopt` force
    p3-wrapup.sh
    o2-finish.sh
elif [ -d 00web/esp ]; then
    echo o2-portal.sh: esp version 1.0 is no longer supported.  Please contact your liaison to upgrade.
    exit 1
#    oracc esp
#    echo You now need to call: oracc esp live to make the rebuilt portal live online
elif [ -e 00web/index.html ] || web-default-index.plx $webdir; then
    mkdir -p $webdir/images
    cp -fpR 00web/* $webdir ; rm -f $webdir/*~
    cp -fp 00lib/thumb.png $webdir
    if [[ "$project" == "cdli" ]]; then
	echo Skipping est processing for project CDLI
    else
	est-project.sh
    fi
    o2-weblive.sh
    o2-finish.sh
else
    echo o2-portal.sh: no portal to rebuild and no index.html or way of building same. Stop.
fi
