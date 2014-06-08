#!/bin/sh

# First harvest all the sigs from the projects' xtfs 
# (i.e., 01bld/from-xtf-glo.sig).
for a in 00map/*.map ; do
    super-getsigs.plx $a
done

# Now treat all of the imported sigs as proxies, which
# they effectively are.
cat 00sig/*.sig >01bld/from-prx-glo.sig

# Now construct a virtual corpus based on the instances
cut -f2 01bld/from-prx-glo.sig | tr ' ' '\n' | cut -d. -f1 | sort -u >00lib/proxy.lst
o2-lst.sh
o2-cat.sh

# Now do a regular glossary build
o2-glossary.sh
