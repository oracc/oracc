#!/bin/sh

proxproj=`cut -d: -f1 < 01bld/lists/proxy-atf.lst | sort -u`
rm -f proxtrans.txt
for a in $proxproj ; do
    grep ^$a 01bld/lists/proxy-atf.lst | perl -pe 's/^.*?:(.*?)\@.*$/$1/' >01tmp/proxtrans.grp
    grep -f 01tmp/proxtrans.grp $ORACC_BUILDS/$a/01bld/translist.txt >>01bld/proxtrans.txt
done
