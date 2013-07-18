#!/bin/sh
if [ "$1" = "" ]; then
    echo esp2-uninstall.sh: must give project name as first argument
    exit 1
fi
[ -r 02www/last_esp2_cp.log ] || (echo esp2-uninstall.sh: no log of last installation ; exit 1)

for a in `cat 02www/last_esp2_cp.log` ; do
    if [ -d $a ]; then
	rm -fr $a
    else 
	if [ -f $a ]; then
	    rm -f $a
	fi
    fi
done
