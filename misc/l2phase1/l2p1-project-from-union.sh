#!/bin/sh
cd 01bld
rm project.sig
union=*/union.sig
set -- $union
grep ^@fields $1 >project.sig
grep -hv '^@\(project\|name\|lang\|fields\)' $union | grep -v ^$ >>project.sig

#for a in [a-z][a-z][a-z]/union.sig ; do
#    grep -v '^@\(project\|name\|lang\)' $a | grep -v '^$' | grep -v '^@fields' >>project.sig
#done
