#!/bin/sh

echo "o2-super-prep.sh: entering at " `date`

# Create 01bld/from-prx-glo.sig
o2-super-prx-sig.sh

# Finally, construct a virtual corpus based on the instances
echo "o2-super-prep.sh: constructing virtual corpus"
grep -v ^@fields 01bld/from-prx-glo.sig | cut -f2 | tr ' ' '\n' \
    | cut -d. -f1 | sort -u | grep -v '^$' >00lib/proxy.lst

catmaster=`oraccopt . catalog-master-project`
if [ "$catmaster" != "" ]; then
    echo "o2-super-prep.sh: using catalog-master-project $catmaster"
    perl -p -i "s/$/\@$catmaster" 00lib/proxy.lst
fi

o2-lst.sh
o2-cat.sh

#xmd-ids.sh -c 01bld/cdlicat.xmd 01bld/lists/cat-ids.lst
echo "o2-super-prep.sh: generating cat-ids.lst"
xmllint --format --encode utf8 01bld/cdlicat.xmd | grep '<id_' \
    | atfgrabpq.plx >01bld/lists/cat-ids.lst

## Now trim the proxy lists to remove any text without xmd info
echo "o2-super-prep.sh: pruning lists"
prune-lists.plx

## And trim the imported signatures as well to remove dead insts
echo "o2-super-prep.sh: pruning sigs"
mkdir -p 01bld/sigbak ; cp -a 01bld/*.sig 01bld/sigbak
l2p1-prune-sigs.plx

## And provide a searchable index of the superglo corpus
mkdir -p 02pub/lem
selemx -p `oraccopt` <01bld/lists/lemindex.lst
chmod o+r 02pub/lem/*

echo "o2-super-prep.sh: exiting at " `date`
