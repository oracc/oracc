#!/bin/sh

echo "o2-super-prep.sh: entering at " `date`

# Create 01bld/from-prx-glo.sig
o2-super-prx-sig.sh
o2-super-pst-prx.sh

echo "o2-super-prep.sh: exiting at " `date`
