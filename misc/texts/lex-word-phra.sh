#!/bin/sh
libscripts=$ORACC_BUILDS/lib/scripts

(cd 01tmp/lex ;
 xsltproc --xinclude $libscripts/lex-data-line.xsl word-master.xml \
	  | xsltproc $libscripts/lex-word-phra-pull.xsl - \
	  >word-phrases.xml ; \
 xsltproc $libscripts/lex-word-phra-group.xsl word-phrases.xml \
	  | xsltproc $libscripts/lex-word-phra-sort.xsl - >word-phra-grouped.xml ; \
     )

xsltproc $libscripts/lex-refs-group.xsl 01tmp/lex/word-phra-grouped.xml \
    | xsltproc $libscripts/lex-data-xis.xsl - >01tmp/lex/word-phra-refs.xml

xsltproc $libscripts/lex-data-xis-ids.xsl 01tmp/lex/word-phra-refs.xml >01tmp/lex/lex.xis

xsltproc $libscripts/xis2tis.xsl 01tmp/lex/lex.xis >02pub/lex.tis

xsltproc $libscripts/lex-data-xis-remap.xsl 01tmp/lex/word-phra-refs.xml \
	 | xsltproc $libscripts/lex-word-phra-HTML.xsl - \
	 >02pub/lex-word-phra.xhtml
