#!/bin/dash
norder=01tmp/neo-order
norderlem=$norder.lem
nordertab=$norder.tab
norderyes=$norder.yes
rm -f $nordertab $norderyes
locate lists/have-lem.lst | xargs ls -1 | sed 's#.*/bld/\(.*\)/lists/have-lem.lst#\1#' >$norderlem
for a in `cat $norderlem` ; do
    printf "%s\t%s\n" $a `oraccopt $a public` >>$nordertab
done
grep yes$ $nordertab | cut -f1 >$norderyes
