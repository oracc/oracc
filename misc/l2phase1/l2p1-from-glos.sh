#!/bin/sh

rebuild_sigs()
{
    echo updating 01bld/from-glos.sig ...
    touch 01bld/nolem 
    l2p1-simple.plx 
    l2p1-cofs.plx
    l2p1-psus.plx 
    cat 01tmp/l2p1-{simple,cofs,psus}.sig >01bld/from-glos.sig 
    exit 0
}

l2p1-needs-update.plx && rebuild_sigs
exit 1
