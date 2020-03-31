#!/bin/sh
for a in /Users/stinney/orc/www/srv/*.tar.xz ; do
    sec=`stat -c %Y $a`
    prj=`basename $a .tar.xz`
    prj=`/bin/echo -n $prj | tr - /`
    echo "$prj	$sec"
done
