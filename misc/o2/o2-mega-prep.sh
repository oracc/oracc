#!/bin/sh

echo "o2-mega-prep.sh: entering at " `date`

# First harvest all the sigs from the projects' xtfs 
# (i.e., 01bld/from-xtf-glo.sig).
echo "o2-mega-prep.sh: getting sigs from public projects"
rm -fr 01sig ; mkdir -p 01sig
megalang=`oraccopt . megalang`
for a in `agg-list-public-projects.sh`; do
    p=$ORACC_BUILDS/$a/01bld/project.sig
    if [ "$megalang" == "" ]; then
	ln -sf $p 01sig
    else
	for l in $megalang ; do
	    afile=`/bin/echo -n $l | tr / -`
	    l2p1-sig-slicer.plx -lang=$l -stdout $a >01sig/$afile-$l.sig
	done
    fi
done

# Now treat all of the imported sigs as proxies, which
# they effectively are.
echo "o2-mega-prep.sh: creating 01bld/from-prx-glo.sig"
echo "@fields sig rank freq inst" >01bld/from-prx-glo.sig
cat 01sig/*.sig >>01bld/from-prx-glo.sig

# Now construct a virtual corpus based on the instances
type=`oraccopt . type`
if [ "$type" = "superglo" ] then
   echo "o2-super-prep.sh: constructing virtual corpus"
   cut -f2 01bld/from-prx-glo.sig | tr ' ' '\n' | cut -d. -f1 | sort -u >00lib/proxy.lst
fi
o2-lst.sh
o2-cat.sh
echo "o2-super-prep.sh: exiting at " `date`
