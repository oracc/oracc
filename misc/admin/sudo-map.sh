#!/bin/sh
cmd=$1
if [ "$cmd" == "" ]; then
    echo sudo-map.sh: must give command to execute over all projects
    exit 1
fi
if [ $(whoami) = "root" ]; then
    for a in `projlist-depth-first.plx` ; do
	user=`/bin/echo -n $a | sed 's,/.*$,,'`
	sudo -u $user -s $ORACC/bin/sudo-map-exec.sh  $ORACC $a $1
    done
else
    echo sudo-map.sh: this program can only be run as root
fi
