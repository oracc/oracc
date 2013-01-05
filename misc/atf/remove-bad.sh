#!/bin/sh
tmp=$1.tmp
rm -f badfiles.lst
echo dummy >$1.log
cp $1.atf $tmp
while [ -s $1.log ]
do
echo error count: `wc -l $1.log`
atfgrabpq.plx $1.log >>badfiles.lst
atfsplit2.plx -cat -except -list badfiles.lst $1.atf >$tmp
atf2xtf -l= -c $tmp
done
