#!/bin/sh
libscripts=$ORACC_BUILDS/lib/scripts

xsltproc $libscripts/lex-word-equi-HTML.xsl 01tmp/lex/lex-word-phra-xis.xml \
	 >01tmp/lex/lex-equi-head.xhtml
xsltproc $libscripts/lex-word-equi-HTML.xsl 01tmp/lex/lex-word-phra-xis-tail.xml \
	 >01tmp/lex/lex-equi-tail.xhtml
xsltproc -stringparam tail-file lex-equi-tail.xhtml $libscripts/lex-merge-phra.xsl 01tmp/lex/lex-equi-head.xhtml \
	 >01tmp/lex/lex-word-equi.xhtml
xsltproc $libscripts/lex-provides.xsl 01tmp/lex/lex-word-equi.xhtml \
	 >01tmp/lex/lex-provides-word-equi.xml
xsltproc -stringparam project `oraccopt` -stringparam type eq \
	 $libscripts/lex-split-HTML.xsl 01tmp/lex/lex-word-equi.xhtml
