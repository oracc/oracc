#!/bin/sh
(cd $ORACC_BUILDS/epsd2 ;
 mkdir -p 00etc
 rm -f 00etc/history-all.edit
 for a in `cat 00lib/superchk.lst` ; do cat $ORACC_BUILDS/$a/00etc/history.edit 2>/dev/null >>00etc/history-all.edit ; done
 for a in epsd2/emesal epsd2/names ; do cat $ORACC_BUILDS/$a/00etc/history.edit 2>/dev/null >>00etc/history-all.edit ; done
 sort -u 00etc/history-all.edit >00etc/history.all
 rm 00etc/history-all.edit )
