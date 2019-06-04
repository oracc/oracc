#!/bin/sh
project=$1
(cd $ORACC_BUILDS/www ; \
 find $project -name 'o[0-9]*.html' | grep -v '\.[fmn]\.' | grep '/[ox][0-9]\+\.' \
     | sed "s#^#$ORACC_BUILDS/www/#" | perl -ne 'm#/([ox][0-9]+)#;  print "$1\t$_"' \
     | sort -u >$ORACC_BUILDS/pub/$project/oid-index.tab
 )
