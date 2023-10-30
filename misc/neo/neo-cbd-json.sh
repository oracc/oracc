#!/bin/sh
wdg=02www/downloads
rm -fr $wdg
mkdir -p $wdg
p=`oraccopt`
g=01bld/[a-z][a-z][a-z]/articles-with-periods.xml
echo $g
GLOSSARIES=$wdg/GLOSSARIES
rm -f $GLOSSARIES
for a in $g; do
    d=`dirname $a`
    l=`basename $d`
    o=$wdg/gloss-$l.json.xz
    echo "running cbd-json.plx $p:$l >$o"
    cbd-json.plx $p:$l | xz --stdout >$o
    echo $o | sed 's#^02www#/neo#' >>$GLOSSARIES
done
chmod o+r $GLOSSARIES
