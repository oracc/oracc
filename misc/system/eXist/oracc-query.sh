#!/bin/sh
if [ -e 00xml/lib/$2.xql ]; then
output=${ORACC}/$1/01tmp/$$-xquery.xml
oracc-ant \
    -Dp.project=$1 \
    -Dp.query=${ORACC}/$1/00xml/lib/$2.xql \
    -Dp.output=$output \
    -f @@ORACC@@/lib/config/eXist-query.xml
cat $output
else
echo oracc-query.sh: no such query 00xml/lib/$2.xql
exit 1
fi
