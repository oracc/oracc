#!/bin/sh
cat $ORACC_BUILDS/lib/data/sx-list-data-head.xml
listnames=`cat 00etc/listnames.lst`
echo '<p class="bookmarks">'
for a in $listnames; do
    l=`/bin/echo $a | tr -d -c a-zA-Z0-9`
    echo "<esp:link bookmark=\"h_$l\">$a</esp:link>"
done
echo '</p>'
for a in $listnames ; do    
    echo "<esp:sh>$a</esp:sh>"
    echo '<div class="listmissing">'
    echo '<pre class="example">'
    cat 00etc/missing-$a.txt
    echo '</pre>'
    echo '</div>'
done
cat $ORACC_BUILDS/lib/data/sx-list-data-tail.xml
