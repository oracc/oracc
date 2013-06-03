#!/bin/sh
proj=$1
if [ "$proj" == "" ]; then
    echo oracc-project-data.sh: must give project on command line
    exit 1
fi
cd $ORACC_HOME
if [ ! -d $proj ]; then
    echo oracc-project-data.sh: no such project $proj. Stop.
    exit 1
fi
fproj=`echo -n $proj | tr / -`
tarball=$ORACC/00any/$fproj-00data.tar.gz
tar --exclude $proj/00any \
    --exclude $proj/00bak \
    --exclude $proj/00web/esp/backups \
    --exclude $proj/00web/esp/httproot_changed \
    --exclude $proj/00web/esp/httproot_dev \
    --exclude $proj/00web/esp/httproot_live \
    --exclude $proj/00web/esp/temporary-files \
    --exclude .DS_Store \
    --exclude '*~' \
    -zcf $tarball $proj/00*
chown oracc:oracc $tarball
