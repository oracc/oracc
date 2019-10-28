#!/bin/sh
if [ -r 00lib/config.xml ]; then
    if [ -d 00etc/stash ]; then
	rm -f *.glo
	rm -f edit.edit
	rm -f to-fix.tab
	rm -fr fix00atf
	cd 00etc/stash || ( echo "$0: cd 00etc/stash failed. Stop." ; exit 1 )
	for a in 20* ; do
	    if [ -d $a ]; then
		echo creating $a.tar.gz and removing $a ...
		tar zcf $a.tar.gz $a
		rm -fr $a
	    fi
	done
    fi
else
    echo "$0: this doesn't look like a project directory. Stop."
    exit 1
fi
