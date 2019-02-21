#!/bin/sh
libscripts=$ORACC_BUILDS/lib/scripts
(cd 01tmp/lex ;
 xsltproc --xinclude $libscripts/lex-sign-group.xsl sign-master.xml >lex-sign-grouped.xml)
xsltproc $libscripts/lex-refs-group.xsl 01tmp/lex/lex-sign-grouped.xml >01tmp/lex/lex-sign-refs.xml
xsltproc $libscripts/lex-sign-HTML.xsl 01tmp/lex/lex-sign-refs.xml >02pub/lex-sign.xhtml
