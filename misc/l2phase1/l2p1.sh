#!/bin/sh
cbdmode=`oraccopt . cbd-mode`
if [ "$cbdmode" != "super" ]; then
    touch 01bld/nolem
    l2p1-from-glos.sh
    [ -s 01bld/lists/have-lem.lst ] && l2p1-from-xtfs.plx -t 01bld/lists/have-lem.lst
    [ -s 01bld/lists/proxy.lst ] && l2p1-from-xtfs.plx -t 01bld/lists/proxy.lst -p
    l2p1-project-sig.plx
    quick=`oraccopt . l2-quick-freqs`
    if [ "$quick" == "yes" ]; then
#	l2p1-proxy-xtf-data.plx  ## THIS MUST BE DEPRECATED
	l2p1-lemm-quick-freqs.plx 
	(cd 02pub ; mv lemm-sux.sig.freq lemm-sux.sig)
    fi
    rm -f 01bld/nolem
fi

l2p1-sig-invert.plx


