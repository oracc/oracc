#!/bin/sh
wdg=02www/downloads
rm -fr $wdg
mkdir -p $wdg
p=`oraccopt`
g=01bld/[a-z][a-z][a-z]/articles-with-periods.xml
echo $g
for a in $g; do
    d=`dirname $a`
    l=`basename $d`
    o=$wdg/gloss-$l.json
    echo "running cbd-json.plx $d:$l >$o"
    cbd-json.plx $p:$l >$o
done
