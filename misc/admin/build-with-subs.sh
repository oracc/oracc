#!/bin/sh
subs=`find . -type d -name 00lib -exec ls '{}/config.xml' ';' | sed 's#/00lib/config.xml##g' | grep -v 00any | sed 's#^./##g' | grep -v '^.$'`

oracc build clean
for a in $subs ; do (cd $a ; oracc resources ; oracc build) ; done
oracc build clean
