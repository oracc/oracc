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

xsltproc $libscripts/lex-data-xis-ids.xsl 01tmp/lex/word-phra-refs.xml >01tmp/lex/word.xis

xsltproc $libscripts/xis2tis.xsl 01tmp/lex/word.xis | tis-sort.plx >02pub/word.tis

xsltproc $libscripts/lex-data-xis-remap.xsl 01tmp/lex/word-phra-refs.xml \
	 >01tmp/lex/lex-word-phra-xis.xml

xsltproc $libscripts/lex-word-phra-HTML.xsl 01tmp/lex/lex-word-phra-xis.xml \
	 >01tmp/lex/lex-phra-head.xhtml

lex-word-tail.sh

xsltproc $libscripts/lex-merge-phra.xsl 01tmp/lex/lex-phra-head.xhtml \
	 >01tmp/lex/lex-word-phra.xhtml

xsltproc $libscripts/lex-provides.xsl 01tmp/lex/lex-word-phra.xhtml \
	 >01tmp/lex/lex-provides-word-phra.xml

xsltproc -stringparam project `oraccopt` -stringparam type wp \
	 $libscripts/lex-split-HTML.xsl 01tmp/lex/lex-word-phra.xhtml
