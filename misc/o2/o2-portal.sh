#!/bin/sh
type=`oraccopt . type`
if [ "$type" == "portal" ]; then
    oracc esp
    echo You now need to call: oracc esp live to make the rebuilt portal live online
else
    echo o2-portal.sh: I only work with portal projects
    exit 1
fi
