#!/bin/sh

echo "o2-super-prep.sh: entering at " `date`

# First harvest all the sigs from the projects' xtfs 
# (i.e., 01bld/from-xtf-glo.sig).
echo "o2-super-prep.sh: getting sigs via 00map"
mkdir -p 01sig
for a in 00map/*.map ; do
    super-getsigs.plx $a
done

# Now treat all of the imported sigs as proxies, which
# they effectively are.
echo "o2-super-prep.sh: creating 01bld/from-prx-glo.sig"
echo "@fields sig inst" >01bld/from-prx-glo.sig
cat 01sig/*.sig >>01bld/from-prx-glo.sig

# Finally, construct a virtual corpus based on the instances
echo "o2-super-prep.sh: constructing virtual corpus"
cut -f2 01bld/from-prx-glo.sig | tr ' ' '\n' | cut -d. -f1 | sort -u >00lib/proxy.lst
o2-lst.sh
o2-cat.sh

xmd-ids.sh -c 01bld/cdlicat.xmd 01bld/lists/cat-ids.lst

## Now trim the proxy lists to remove any text without xmd info

echo "o2-super-prep.sh: exiting at " `date`
