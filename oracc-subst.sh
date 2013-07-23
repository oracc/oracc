#!/bin/sh
ORACC=$1
in=$2
out=`basename $in .in`
pat1="s#\@\@ORACC\@\@#${ORACC}#g"
pat2="s#\@\@ORACC_HOST\@\@#${ORACC_HOST}#g"
pat="$pat1;$pat2"
rm -f $out
perl -C0 -pe "$pat" <$in >$out
chmod -w $out
