#!/bin/sh

function history {
    h=$ORACC_BUILDS/$1/00etc/history.edit
    if [ -r $h ]; then
	echo "### $h" >>00etc/history-all.edit
	cat $h >>00etc/history-all.edit
    fi
}

(cd $ORACC_BUILDS/epsd2 ;
 mkdir -p 00etc
 rm -f 00etc/history-all.edit
 for a in `cat 00lib/superchk.lst` ; do history $a ; done
 for a in epsd2/emesal epsd2/names ; do history $a ; done
 grep -v '^###' 00etc/history-all.edit | sort -u >00etc/history.all
)
