#!/bin/sh
webdir=$1
for a in 00lib/lists/* ; do 
    tr '\r' '\n' <$a | tr -s '\n' >$webdir/`basename $a`
done

