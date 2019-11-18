#!/bin/sh

# now that g2 is default this script is not used

exit 0

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

# With G2 from-glos.sig is built directly by cbdpp.plx
g2=`oraccopt . g2`
if [ "$g2" == "yes" ]; then
    exit 0
fi

projtype=`oraccopt . type`
if [ "$projtype" == "superglo" ]; then
    l2p1-needs-update.plx
    rebuild_sigs
else
    l2p1-needs-update.plx && rebuild_sigs
fi
exit 1
