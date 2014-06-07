#!/bin/sh
for a in 00map/*.map ; do
    super-getsigs.plx $a
done
cat 00sig/*.sig >01bld/project.sig
o2-glossary.sh
