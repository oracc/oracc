#!/bin/sh
rm -f 00etc/epsd-atf.lst
for a in `allprojlist.sh` ; do
    (cd $ORACC_BUILDS/$a ; atfgrabpq.plx -proj $a -atf 00atf/*.atf >>$ORACC_BUILDS/atf.lst)
done
