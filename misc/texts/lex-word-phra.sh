#!/bin/sh
libscripts=$ORACC_BUILDS/lib/scripts
(cd 01tmp/lex ;
 xsltproc --xinclude $libscripts/lex-word-phra-group.xsl word-master.xml \
	  >word-phra-grouped.xml)
xsltproc $libscripts/lex-refs-group.xsl 01tmp/lex/word-phra-grouped.xml \
	 >01tmp/lex/word-phra-refs.xml
exit 1
xsltproc $libscripts/lex-word-phra-HTML.xsl 01tmp/lex/word-phra-refs.xml \
	 >02pub/lex-word-phra.xhtml
