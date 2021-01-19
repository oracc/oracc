#!/bin/sh
echo inline-senses=yes
webdir=$1
l=$2
proj=`oraccopt`
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
snsdir=$webdir/cbd/$l/sns
rm -fr $snsdir
mkdir -p $snsdir
xislist=01tmp/$l-xis.lst
xl 01bld/sux/articles.xml | grep -F '<sense ' | perl -p -e 's/^.*?xis=\"(.*?)\".*$/$1/' >$xislist
while IFS= read -r xis
do
    out=$snsdir/$xis.html
    # echo "$0: generating $out"
    xis-xml-page.sh $proj $l $xis | xis-xml-html.sh $proj >$out
done < $xislist
chmod +rx $snsdir
chmod -R o+r $snsdir
echo after inline-senses
