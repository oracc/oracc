#!/bin/sh
type=`oraccopt . type`
if [ "$type" == "portal" ]; then
    esp2=`oraccopt . esp2`
    if [ "$esp2" == "yes" ]; then
	esp2.sh `oraccopt`
    else
	oracc esp
	echo You now need to call: oracc esp live to make the rebuilt portal live online
    fi
else
    echo o2-portal.sh: I only work with portal projects
    exit 1
fi
