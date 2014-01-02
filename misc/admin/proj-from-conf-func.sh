#!/bin/sh
function proj_from_conf {
    if [ -r 00lib/subconfig.xml ]; then
	fig=00lib/subconfig.xml
    elif [ -r 00lib/config.xml ]; then
	fig=00lib/config.xml
    else
	fig=""
    fi
    if [ "$fig" = "" ]; then
	/bin/echo -n ''
    else
	/bin/echo -n `perl -pe 's/^<.xml.*>//' $fig | tr '\n' ' ' | perl -ne '/n="(.*?)"/ && print $1'`
    fi
}
