#!/bin/sh
echo xff=yes
webdir=$1
l=$2
if [ "$webdir" == "" ]; then
    echo "$0: usage: WEBDIR LANG. Stop"
    exit 1
fi
if [ "$l" == "" ]; then
    echo "$0: usage: WEBDIR LANG. Stop"
    exit 1
fi
if [ ! -r 01bld/$l/articles.xml ]; then
    echo "$0: 01bld/$l/articles.xml does not exist. Stop"
    exit 1
fi
xffdir=$webdir/cbd/$l/xff
rm -fr $xffdir
mkdir -p $xffdir
art2xff.plx -d $xffdir -l $l
chmod +rx $webdir/cbd/$l/xff
chmod -R o+r $webdir/cbd/$l/xff
echo after art2xff
