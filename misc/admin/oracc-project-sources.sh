#!/bin/sh
function fail {
    echo oracc-project-sources.sh: $*. Stop.
    exit 1
}
proj=$1
if [ "$proj" == "" ]; then
    echo oracc-project-sources.sh: must give project on command line
    exit 1
fi
cd $ORACC/$proj || fail "no such project $proj"
fproj=`/bin/echo -n $proj | tr / -`
tarball=01tmp/$fproj-$2.tar.gz
echo oracc-project-sources.sh: creating $tarball
tar --exclude 00any \
    --exclude 00bak \
    --exclude 00web/esp/backups \
    --exclude 00web/esp/httproot_changed \
    --exclude 00web/esp/httproot_changed_new \
    --exclude 00web/esp/httproot_dev \
    --exclude 00web/esp/httproot_gone \
    --exclude 00web/esp/httproot_gone_new \
    --exclude 00web/esp/httproot_live \
    --exclude 00web/esp/httproot_live_new \
    --exclude 00web/esp/temporary-files \
    --exclude .DS_Store \
    --exclude '*~' \
    --exclude '*.bak' \
    --exclude '*.tar.gz' \
    -zcf $tarball 00*
