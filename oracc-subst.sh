#!/bin/sh
ORACC=$1
in=$2
out=`basename $in .in`
pat="s#\@\@ORACC\@\@#${ORACC}#g"
rm -f $out
perl -C0 -pe $pat <$in >$out
chmod -w $out
