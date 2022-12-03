#!/bin/sh
#
# Usage:
#   cd to a project directory
#   update-00res.sh
#
if [ ! -d 00res ]; then
    echo $0: no 00res directory to update. Stop
    exit 1
fi
p=`proj-from-conf.sh`
if [ -d 00res/downloads ]; then
    echo $0: updating 00res/downloads
    (cd 00res/downloads ;
     d=`pwd`
     cd $ORACC_BUILDS/www/$p/downloads ;
     rsync -vic $d/* . )
else
    echo $0: no 00res/downloads directory, skipping.
fi
