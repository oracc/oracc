#!/bin/sh
ORACC=$1
in=$2
out=`basename $in .in`
pat="s#\@\@ORACC\@\@#${ORACC}#g"
perl -C0 -pe $pat <$in >$out
