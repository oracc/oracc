#!/bin/sh
#echo o2-unbuild.sh: currdir=`pwd`
#
# STEVE! If you ever think it is a good idea to rm
# 02pub as part of unbuild remember that
# `l2p1-lemm-data.plx -update' relies on 02pub/lemm-*
#
if [ -d 01bld ]; then 
    cd 01bld
    find . -maxdepth 1 -type f -exec rm '{}' ';'
    rm -fr [PQX][0-9][0-9][0-9]
    rm -fr [a-z][a-z][a-z]
    rm -fr *-x-* lists/*
    cd ..
fi
[ -d 01tmp ] && (cd 01tmp && rm -fr *.*)
