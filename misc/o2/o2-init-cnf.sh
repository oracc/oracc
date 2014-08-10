#!/bin/sh
d=`pwd`
echo o2-init-cnf called from $d >2
ls -l 00lib/config.xml
o2-cnf.sh
