#!/bin/sh
if [ $(whoami) = "root" ]; then
    for a in `agg-list-public-projects.sh` ; do
	user=`echo -n $a | sed 's,/.*$,,'`
	if [ "$user" != "cdli" ]; then
	    echo Rebuilding full text index of $a ...
	    sudo -u $user -s $ORACC/bin/est-sudo.sh $ORACC $a
	fi
    done
else
    echo est-all-projects.sh: this program can only be run as root
fi
