#!/bin/sh
ORACC=$1
in=$2
out=`basename $in .in`
perl -C0 -pe "s#\@\@ORACC\@\@#$ORACC#g" <$in >$out
