#!/bin/dash
norder=01tmp/neo-order
norderlem=$norder.lem
nordertab=$norder.tab
norderyes=$norder.yes
rm -f $nordertab $norderyes
for a in `locate lists/have-lem.lst` ; do
    if [ -s $a ]; then
	/bin/echo $a | sed 's#.*/bld/\(.*\)/lists/have-lem.lst#\1#' >>$norderlem
    fi
done
for a in `cat $norderlem` ; do
    printf "%s\t%s\n" $a `oraccopt $a public` >>$nordertab
done
grep yes$ $nordertab | cut -f1 >$norderyes
