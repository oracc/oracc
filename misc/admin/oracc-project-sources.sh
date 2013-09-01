#!/bin/sh
proj=$1
if [ "$proj" == "" ]; then
    echo oracc-project-sources.sh: must give project on command line
    exit 1
fi
cd $ORACC_HOME
if [ ! -d $proj ]; then
    echo oracc-project-sources.sh: no such project $proj. Stop.
    exit 1
fi
fproj=`/bin/echo -n $proj | tr / -`
tarball=$ORACC/01tmp/$fproj.tar.gz
tar --exclude $proj/00any \
    --exclude $proj/00bak \
    --exclude $proj/00web/esp/backups \
    --exclude $proj/00web/esp/httproot_changed \
    --exclude $proj/00web/esp/httproot_changed_new \
    --exclude $proj/00web/esp/httproot_dev \
    --exclude $proj/00web/esp/httproot_gone \
    --exclude $proj/00web/esp/httproot_gone_new \
    --exclude $proj/00web/esp/httproot_live \
    --exclude $proj/00web/esp/httproot_live_new \
    --exclude $proj/00web/esp/temporary-files \
    --exclude .DS_Store \
    --exclude '*~' \
    --exclude '*.bak' \
    --exclude '*.tar.gz' \
    -zcf $tarball $proj/00*
chown oracc:oracc $tarball
