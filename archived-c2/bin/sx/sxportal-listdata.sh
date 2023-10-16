#!/bin/sh

#
# Build list-data.xml
#
f=00web/list-data.xml
cat $ORACC_BUILDS/lib/data/sx-list-head.xml >$f
listnames=`cat 00etc/listnames.lst`
echo '<p class="bookmarks">' >>$f
for a in $listnames; do
    l=`/bin/echo $a | tr -d -c a-zA-Z0-9 | tr 'A-Z' 'a-z'`
    echo "<esp:link bookmark=\"h_$l\">$a</esp:link>" >>$f
done
echo '</p>' >>$f
for a in $listnames ; do    
    echo "<esp:sh>$a</esp:sh>" >>$f
    echo '<div class="listmissing">' >>$f
    echo '<pre class="example">' >>$f
    cat 00etc/missing-$a.txt >>$f
    echo '</pre>' >>$f
    echo '</div>' >>$f
done
cat $ORACC_BUILDS/lib/data/sx-list-tail.xml >>$f
