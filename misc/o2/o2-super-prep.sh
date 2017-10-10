#!/bin/sh

# First harvest all the sigs from the projects' xtfs 
# (i.e., 01bld/from-xtf-glo.sig).
for a in 00map/*.map ; do
    super-getsigs.plx $a
done

# Now treat all of the imported sigs as proxies, which
# they effectively are.
cat 00sig/*.sig >01bld/from-prx-glo.sig

# Create lemm-LANG.sig files then freq files
langs=`oraccopt . cbd-super-list`
for l in $langs ; do
    echo "superglo: creating 02pub/lemm-$l.sig"
    grep "%$l:" 01bld/from-glos.sig >02pub/lemm-$l.sig
done
l2p1-lemm-quick-freqs.plx epsd2
for a in 02pub/*.freq  ; do
    mv $a `basename $a .freq`
done

# Now construct a virtual corpus based on the instances
cut -f2 01bld/from-prx-glo.sig | tr ' ' '\n' | cut -d. -f1 | sort -u >00lib/proxy.lst
o2-lst.sh
o2-cat.sh
