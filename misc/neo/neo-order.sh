#!/bin/dash
norder=01tmp/neo-order
nordertab=$norder.tab
norderyes=$norder.yes
rm -f norder
for a in `cat have-lem` ; do
    printf "%s\t%s\n" $a `oraccopt $a public` >>$nordertab
done
grep yes$ $nordertab | cut -f1 >$norderyes
