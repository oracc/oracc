#!/bin/sh
libscripts=$ORACC_BUILDS/lib/scripts
(cd 01tmp/lex ;
 xsltproc --xinclude $libscripts/lex-word-base-group.xsl word-master.xml \
	  >word-base-grouped.xml)
xsltproc $libscripts/lex-refs-group.xsl 01tmp/lex/word-base-grouped.xml \
	 >01tmp/lex/word-base-refs.xml
xsltproc $libscripts/lex-word-base-HTML.xsl 01tmp/lex/word-base-refs.xml \
	 >02pub/lex-word-base.xhtml
