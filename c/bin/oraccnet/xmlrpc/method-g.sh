#!/bin/sh
echo '%%' >methods.g
for a in m_*.c; do
    m=`basename $a .c | cut -d_ -f2`
    echo "$m, ${m}_call, ${m}_resp, ${m}_wait, NULL" >>methods.g
done
