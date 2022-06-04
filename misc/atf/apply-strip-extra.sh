#!/bin/sh
for a in $* ; do
    d=`dirname $a` ;
    b=`basename $a` ;
    l=`basename $a .atf`.log ;
    (cd $d ; mkdir -p bak log ; mv $b bak/ ; atf-strip-sux-extra.plx bak/$b >$b 2>log/$l)
done
