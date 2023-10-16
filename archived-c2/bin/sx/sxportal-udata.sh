#!/bin/sh

#
# Build u-data.xml
#
sxudatacodes=$ORACC_BUILDS/lib/data/sx-udata-codes.tsv
f=00web/u-data.xml
cat $ORACC_BUILDS/lib/data/sx-udata-head.xml >$f
headnames=`cut -f2 $sxudatacodes`
echo '<p class="bookmarks">' >>$f
for a in $headnames; do
    l=`/bin/echo $a | tr -d -c a-zA-Z0-9 | tr A-Z a-z`
    p=`/bin/echo $a | tr _ ' '`
    echo "<esp:link bookmark=\"h_$l\">$p</esp:link>" >>$f
done
echo '</p>' >>$f
codes=(`cut -f1 $sxudatacodes`)
names=(`cut -f2 $sxudatacodes`)
for i in ${!codes[@]}; do
    c=${codes[$i]}
    n=`/bin/echo ${names[$i]} | tr _ ' '`
    x=00etc/udata-$c.xml
    echo "<esp:sh>$n</esp:sh>" >>$f
    if [ "$c" == "need" ]; then
	nn=`grep -c ^need 00etc/u-data.tsv`
	echo "<p>The following list of characters which might need encoding contains $nn entries.</p>" >>$f
    fi
    echo '<table style="font-size: 80%" class="pretty">' >>$f
    cat $x >>$f
    echo '</table>' >>$f
done
cat $ORACC_BUILDS/lib/data/sx-list-tail.xml >>$f
