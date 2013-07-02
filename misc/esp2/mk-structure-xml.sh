#!/bin/sh
LIST=`find . -name '*.xml'`
for a in $LIST; do
    id=`perl -n -e 'm,<esp:name>(.*?)</esp:name>,&&print $1' $a`
    fn=`echo $a | sed s/^..//`
    echo $id'	'$fn
done
