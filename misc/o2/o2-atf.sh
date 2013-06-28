#!/bin/sh
l2p1-pre-atf.sh
# rebuild files that have changed
o2-atfsources.plx
o2-atf.plx
LIST=`(cd 00atf ; find . -type f -name '*.atf' -print0 | xargs -0 grep -l '^@score' | head -1)`
if [ "$LIST" != "" ]; then
    o2-unscore.sh
fi
