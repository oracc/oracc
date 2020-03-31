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
		echo creating $a.tar.xz and removing $a ...
		tar Jcf $a.tar.xz $a
		rm -fr $a
	    fi
	done
	rm -f $ORACC_BUILDS/epsd2/new*.glo
	rm -f $ORACC_BUILDS/epsd2/emesal/new*.glo
	rm -f $ORACC_BUILDS/epsd2/names/new*.glo
    fi
else
    echo "$0: this doesn't look like a project directory. Stop."
    exit 1
fi
