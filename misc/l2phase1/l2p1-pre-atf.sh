#!/bin/sh
otype=`oraccopt . type`
if [ $otype == "superglo" ]; then
    o2-super-prep.sh
fi
l2p1-from-glos.sh && l2p1-project-sig.plx -l
