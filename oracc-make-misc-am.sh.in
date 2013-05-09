#!/bin/sh
cd @@ORACC@@/src/misc
rm */Makefile.am
for a in * ; do [ -d $a ] && (cd $a ; ../../oracc-misc-am.plx $a ) ; done
