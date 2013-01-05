#!/bin/sh
yesno=$1
if [ "$yesno" == "yes" ] || [ "$yesno" == "no" ]; then
    if [ -r 00lib/config.xml ] && [ -w 00lib/config.xml ]; then
	xsltproc -o 00lib/config.xml \
	    -stringparam public $1 ${ORACC}/lib/scripts/o2-public.xsl \
	    00lib/config.xml
	o2-cnf.sh
    else
	echo o2-public.sh: unable to read/write 00lib/config.xml
	exit 1
    fi
else
    echo o2-public.sh: please say 'yes' or 'no', not "'"$1"'"
    exit 1
fi

