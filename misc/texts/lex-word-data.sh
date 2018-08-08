#!/bin/sh
projects=$*
if [ "$projects" == "" ]; then
    echo 'lex-word-data.sh: must give project(s) on command line'
    exit 1
fi
rm -fr 01tmp/lex/word
mkdir -p 01tmp/lex/word
for p in $* ; do
    withlem=$ORACC_BUILDS/$p/01bld/lists/withlem
    for a in `pqxpand xtf <01bld/lists/withlem | xargs grep -l -F 'subtype="wp"'`; do
       	lex-word-xtf.sh $a
    done
done
lex-master.sh word
libscripts=$ORACC_BUILDS/lib/scripts
(cd 01tmp/lex ;
 xsltproc --xinclude $libscripts/lex-word-group.xsl lex-word-master.xml >lex-word-grouped.xml)
xsltproc $libscripts/lex-word-HTML.xsl 01tmp/lex/lex-word-grouped.xml >02pub/lex-word.xhtml
