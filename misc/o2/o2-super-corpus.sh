#!/bin/sh

echo "o2-super-corpus.sh: entering at " `date`

# Construct a virtual corpus based on the instances from from-prx-glo.sig
echo "o2-super-corpus.sh: constructing virtual corpus"
grep -v ^@fields 01bld/from-prx-glo.sig | cut -f2 | tr ' ' '\n' | cut -d. -f1 | sort -u >00lib/proxy.lst
o2-lst.sh
o2-cat.sh

echo "o2-super-corpus.sh: generating cat-ids.lst"
xmllint --format --encode utf8 01bld/cdlicat.xmd | grep '<id_' \
    | atfgrabpq.plx >01bld/lists/cat-ids.lst

## Now trim the proxy lists to remove any text without xmd info
echo "o2-super-corpus.sh: pruning lists"
prune-lists.plx

## And trim the imported signatures as well to remove dead insts
echo "o2-super-corpus.sh: pruning sigs"
mkdir -p 01bld/sigbak ; cp -a 01bld/*.sig 01bld/sigbak
l2p1-prune-sigs.plx 01bld/from-prx-glo.sig

echo "o2-super-corpus.sh: exiting at " `date`
