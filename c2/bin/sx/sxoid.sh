#!/bin/sh
#
# List all the signlist OIDs in oid.tab.
#
grep '	sl	' $ORACC_BUILDS/oid/oid.tab | cut -f1
