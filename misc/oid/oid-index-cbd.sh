#!/bin/sh
project=$1
if [ "$project" == "" ]; then
    echo "$0: must give project on command line. Stop."
    exit 1
fi
(cd $ORACC_BUILDS ; \
 find www/$project -maxdepth 3 -name 'o[0-9]*.html' | grep -v '\.[0-9fmn]' | grep '/[ox][0-9]\+\.' \
     | perl -ne 'm#/([ox][0-9]+)#;  print "$1\t$_"' \
     | sort -u >$ORACC_BUILDS/pub/$project/oid-index.tab
)
chmod o+r $ORACC_BUILDS/pub/$project/oid-index.tab

#      | sed "s#^#$ORACC_BUILDS/www/#" |
