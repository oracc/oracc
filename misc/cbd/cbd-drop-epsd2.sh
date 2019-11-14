#!/bin/sh
proj=$1
lang=$2
if [ "$proj" == "" -o "$lang" == "" ]; then
    echo "$0: must give project and lang on command line. Stop."
    exit 1
fi
grp="@${proj}%${lang}:"
tab=proj-$lang.tab
glo=proj-$lang.glo
echo grp=$grp
ox -4 -I01bld/atfsources.lst | grep $grp | grep -v epsd2 >$tab
cut -f2 <$tab | l2p2-sig-g2a.plx -filter -lang $lang -proj $proj >$glo
