#!/bin/sh
projects=$*
if [ "$projects" == "" ]; then
    echo 'lex-sign-data.sh: must give project(s) on command line'
    exit 1
fi
mkdir -p 01tmp/lex/sign
for p in $* ; do
    withlem=$ORACC_BUILDS/$p/01bld/lists/withlem
    for a in `head $withlem` ; do
	lex-sign-xtf.sh `pqxpand xtf $a`
    done
done
lex-master.sh sign
(cd 01tmp/lex ;
 xsltproc lex-sign-group.xsl lex-sign-master.xml >lex-sign-grouped.xml ;
 xsltproc lex-sign-HTML.xsl lex-sign-grouped.xml >lex-sign.xhtml)
