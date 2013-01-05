#!/bin/sh
if [ "$1" = "" ]; then
    echo config-inherit.sh: must give project to inherit from
    exit 1
fi
self=`oraccopt`
if [ "$1" = "$self" ]; then
    echo config-inherit.sh: inheriting from self not allowed
    exit 1
fi
if [ ! -r $ORACC/xml/$1/config.xml ]; then
    echo config-inherit.sh: no $ORACC/xml/$1/config.xml to inherit from
    exit 1
fi
if [ -r 00lib/config.xml ]; then
    cp -p 00lib/config.xml 00any/config-pre-inherit.xml || exit 1
    rm -f 00lib/config.xml
    xsltproc -stringparam inherit-from $1 $ORACC/lib/scripts/config-inherit.xsl \
	00any/config-pre-inherit.xml \
	>00lib/subconfig.xml || exit 1
    (cd 00lib; ln -sf subconfig.xml config.xml)
else
    echo config-inherit.sh: I only work in a project or subproject directory
fi
