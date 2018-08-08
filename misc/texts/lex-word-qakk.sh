#!/bin/sh
libscripts=$ORACC_BUILDS/lib/scripts
(cd 01tmp/lex ;
 xsltproc --xinclude $libscripts/lex-word-qakk-group.xsl word-master.xml \
	  >word-qakk-grouped.xml)
xsltproc $libscripts/lex-refs-group.xsl 01tmp/lex/word-qakk-grouped.xml \
	 >01tmp/lex/word-qakk-refs.xml
exit 1
xsltproc $libscripts/lex-word-qakk-HTML.xsl 01tmp/lex/word-qakk-refs.xml \
	 >02pub/lex-word-qakk.xhtml
