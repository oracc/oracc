#!/bin/sh
o2-prepare.sh
buildpolicy=`oraccopt . build-approved-policy`;
if [ "$buildpolicy" != "search" ]; then
    umbrella=`oraccopt . cbd-super`;
    if [ "$umbrella" = "umbrella" ]; then
	echo "o2-corpus.sh: getting sigs via umbrella.lst"
	for a in `cat 00lib/umbrella.lst` ; do
	    grep '%' $a/01bld/from-xtf-glo.sig >>01bld/from-prx-glo.sig
	done
    else
	echo "o2-corpus.sh: getting sigs from corpus"
	[ -s 01bld/lists/have-lem.lst ] && l2p1-from-xtfs.plx -t 01bld/lists/have-lem.lst
	[ -s 01bld/lists/proxy-lem.lst ] && l2p1-from-xtfs.plx -proxy -t 01bld/lists/proxy-lem.lst
    fi
    o2-glo.sh
fi
o2-xtf.sh $*
#o2-web.sh
o2-web-corpus.sh
#o2-prm.sh
o2-weblive.sh
o2-finish.sh
