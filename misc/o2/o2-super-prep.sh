#!/bin/sh

echo "o2-super-prep.sh: entering at " `date`
# First harvest all the sigs from the projects' xtfs 
# (i.e., 01bld/from-xtf-glo.sig).
echo "@fields sig inst" >01bld/from-prx-glo.sig
umbrella=`oraccopt . cbd-super`;
if [ "$umbrella" = "umbrella" ]; then
    echo "o2-super-prep.sh: getting sigs via umbrella.lst"
    for a in `cat 00lib/umbrella.lst` ; do
	grep '%' $a/01bld/from-xtf-glo.sig >>01bld/from-prx-glo.sig
    done
else
    echo "o2-super-prep.sh: getting sigs via 00map"
    mkdir -p 01sig
    for a in 00map/*.map ; do
	super-getsigs.plx $a
    done
    # Now treat all of the imported sigs as proxies, which
    # they effectively are.
    echo "o2-super-prep.sh: creating 01bld/from-prx-glo.sig"
    cat 01sig/*.sig >>01bld/from-prx-glo.sig
fi

# get the sigs from the super glossary
echo "o2-super-prep.sh: running l2p1-from-glos.sh"
l2p1-from-glos.sh

# Create lemm-LANG.sig files then freq files
echo "o2-super-prep.sh: creating lemm-XXX.sig"
proj=`oraccopt`
langs=`oraccopt . cbd-super-list`
for l in $langs ; do
    echo "superglo: creating 02pub/lemm-$l.sig"
    grep "%$l:" 01bld/from-glos.sig >02pub/lemm-$l.sig
done
l2p1-lemm-quick-freqs.plx $proj
for a in 02pub/*.freq  ; do
    sort -t'	' -nrk3 $a >02pub/`basename $a .freq`
done

# Now construct a virtual corpus based on the instances
type=`oraccopt . type`
if [ "$type" = "superglo" ] then
   echo "o2-super-prep.sh: constructing virtual corpus"
   cut -f2 01bld/from-prx-glo.sig | tr ' ' '\n' | cut -d. -f1 | sort -u >00lib/proxy.lst
fi
o2-lst.sh
o2-cat.sh
echo "o2-super-prep.sh: exiting at " `date`
