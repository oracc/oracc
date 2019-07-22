#!/bin/sh
libscripts=$ORACC_BUILDS/lib/scripts

(cd 01tmp/lex ; \
 xsltproc $libscripts/lex-pull-tail.xsl word-phrases.xml >phra-tail.xml
 xsltproc $libscripts/lex-phra-tail-group.xsl phra-tail.xml \
     | xsltproc $libscripts/lex-word-phra-sort.xsl - >phra-tail-grouped.xml ; \
     )

xsltproc $libscripts/lex-refs-group.xsl 01tmp/lex/phra-tail-grouped.xml \
    | xsltproc $libscripts/lex-data-xis.xsl - >01tmp/lex/phra-tail-refs.xml

xsltproc  --stringparam idbase tail $libscripts/lex-data-xis-ids.xsl \
	  01tmp/lex/phra-tail-refs.xml >01tmp/lex/tail.xis

xsltproc $libscripts/xis2tis.xsl 01tmp/lex/tail.xis | tis-sort.plx >02pub/tail.tis

xsltproc $libscripts/lex-data-xis-remap.xsl 01tmp/lex/phra-tail-refs.xml >01tmp/lex/lex-word-phra-xis-tail.xml

xsltproc $libscripts/lex-word-phra-HTML.xsl 01tmp/lex/lex-word-phra-xis-tail.xml >01tmp/lex/lex-phra-tail.xhtml
