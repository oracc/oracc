#!/bin/sh
libscripts=$ORACC_BUILDS/lib/scripts

(cd 01tmp/lex ;
 xsltproc $libscripts/lex-pull-base.xsl word-phrases.xml \
     | xsltproc $libscripts/lex-phra-base-group.xsl - \
     | xsltproc $libscripts/lex-sort.xsl - >phra-base-grouped.xml ; \
     )

xsltproc $libscripts/lex-refs-group.xsl 01tmp/lex/phra-base-grouped.xml \
    | xsltproc $libscripts/lex-data-xis.xsl - >01tmp/lex/phra-base-refs.xml

xsltproc --stringparam idbase base $libscripts/lex-data-xis-ids.xsl 01tmp/lex/phra-base-refs.xml >01tmp/lex/base.xis

xsltproc $libscripts/xis2tis.xsl 01tmp/lex/base.xis >02pub/base.tis

xsltproc --stringparam xis-file base.xis $libscripts/lex-data-xis-remap.xsl 01tmp/lex/phra-base-refs.xml \
	 >01tmp/lex/phra-base-remapped.xml

xsltproc $libscripts/lex-phra-base-HTML.xsl 01tmp/lex/phra-base-remapped.xml \
	 >02pub/lex-phra-base.xhtml
