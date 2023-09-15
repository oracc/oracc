#!/bin/sh
p=`oraccopt`
w=${ORACC_BUILDS}/$p/02www
if [ -d 01tmp/xis ]; then
    (cd 01tmp/xis ;
     cp */*.tis $w/lists
     rm -fr $w/inst ; mkdir -p $w/inst
     cp */*.html $w/inst
     chmod -R o+r $w/lists
     chmod -R o+r $w/inst
     )
fi
