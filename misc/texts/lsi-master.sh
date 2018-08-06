#!/bin/sh
echo "<lsi:dataset xmlns:xi=\"http://www.w3.org/2001/XInclude\" xmlns:lsi=\"http://oracc.org/ns/lsi/1.0\">" >01tmp/lsi-master.xml
cd 01tmp
for a in `find lsi -type f` ; do
    xi="<xi:include href=\"$a\"/>"
    echo $xi >>lsi-master.xml
done
cd ..
echo '</lsi:dataset>' >>01tmp/lsi-master.xml
