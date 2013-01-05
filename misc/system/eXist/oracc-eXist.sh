#!/bin/sh
oracc-ant -Dp.project=$1 -Dp.project.user=$2 -Dp.project.pass=`cat ~/.exist` \
    -f @@ORACC@@/lib/config/eXist-remove.xml
oracc-ant -Dp.project=$1  -Dp.project.user=$2 -Dp.project.pass=`cat ~/.exist` \
    -f @@ORACC@@/lib/config/eXist-store.xml
