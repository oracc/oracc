#!/bin/sh
cd 02www
for a in `grep -l -m1 -r 'generator. content=.Oracc ESP' *` ; do
    # paranoia-check to avoid doing rm -fr on an empty string ...
    d=`dirname $a`
    if [ "$d" != "" ]; then
	echo removing $d
	rm -fr $d
    fi
done
