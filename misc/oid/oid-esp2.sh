#!/bin/bash
project=`oraccopt`
find 02www/ -name '[ox][0-9][0-9][0-9][0-9][0-9][0-9][0-9]' -type d | \
    sed 's#^02www##' >01tmp/oid-esp-f.tmp
sed 's#^.*/\([ox][0-9]*\)$#\1#' <01tmp/oid-esp-f.tmp >01tmp/oid-oid-o.tmp
sed "s#^#u	/$project#" <01tmp/oid-esp-f.tmp | \
    paste 01tmp/oid-oid-o.tmp - >02pub/oid-index.tab
