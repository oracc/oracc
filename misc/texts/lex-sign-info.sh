#!/bin/sh
mkdir -p 01tmp/lsi
for p in $* ; do
    withlem=$ORACC_BUILDS/$p/01bld/lists/withlem
    for a in `head $withlem` ; do
	lsi-one-xtf.sh `pqxpand xtf $a`
    done
done
lsi-master.sh
