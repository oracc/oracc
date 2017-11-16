#!/bin/sh

echo "o2-mega-prep.sh: entering at " `date`

# create a list of lemmatized files, discarding duplicates
neo-proxies.plx

# collect all the signatures, dropping instances from discarded
# duplicates, then dropping signatures with no instances
neo-sigs.plx

# create a glossary for each language in the mega's project.sig


# Now treat all of the imported sigs as proxies, which
# they effectively are.
echo "o2-mega-prep.sh: creating 01bld/from-prx-glo.sig"
echo "@fields sig rank freq inst" >01bld/from-prx-glo.sig
cat 01sig/*.sig >>01bld/from-prx-glo.sig

## Now construct a virtual corpus based on the instances
#type=`oraccopt . type`
#if [ "$type" = "superglo" ] then
#   echo "o2-super-prep.sh: constructing virtual corpus"
#   cut -f2 01bld/from-prx-glo.sig | tr ' ' '\n' | cut -d. -f1 | sort -u >00lib#/proxy.lst
#fi
#o2-lst.sh
#o2-cat.sh
#echo "o2-super-prep.sh: exiting at " `date`
