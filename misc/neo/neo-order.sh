#!/bin/dash
norder=01tmp/neo-order
norderlem=$norder.lem
nordertab=$norder.tab
norderyes=$norder.yes
rm -f $norderlem $nordertab $norderyes
for a in `locate lists/have-lem.lst` ; do
    if [ -s $a ]; then
	/bin/echo $a | sed 's#.*/bld/\(.*\)/lists/have-lem.lst#\1#' >>$norderlem
    fi
done
for a in `cat $norderlem` ; do
    public=`oraccopt $a public`
    printf "%s\t%s\n" $a $public >>$nordertab
done
grep yes$ $nordertab | cut -f1 >$norderyes
echo $0: The following projects have lemmatized files but are not in 00lib/order.lst:
grep -x -v -f 00lib/order.lst 01tmp/neo-order.yes
