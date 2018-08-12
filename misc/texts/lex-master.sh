#!/bin/sh
type=$1
if [ "$type" == "" ]; then
    echo 'lex-master.sh: must give type, e.g., sign'
    exit 1
fi
project=`oraccopt`
echo "<lex:dataset project=\"$project\" xmlns:xi=\"http://www.w3.org/2001/XInclude\" xmlns:lex=\"http://oracc.org/ns/lex/1.0\">" \
     >01tmp/lex/$type-master.xml
cd 01tmp/lex
for a in `find $type -type f` ; do
    xi="<xi:include href=\"$a\"/>"
    echo $xi >>$type-master.xml
done
cd ../..
echo '</lex:dataset>' >>01tmp/lex/$type-master.xml
