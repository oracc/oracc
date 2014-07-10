#!/bin/sh
cd 01bld
rm project.sig
for a in [a-z][a-z][a-z]/union.sig ; do
    grep -v '^@\(project\|name\|lang\)' $a| grep -v '^$' >>project.sig
done
