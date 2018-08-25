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

xsltproc $libscripts/xis2tis.xsl 01tmp/lex/base.xis | tis-sort.plx >02pub/base.tis

xsltproc --stringparam xis-file base.xis $libscripts/lex-data-xis-remap.xsl 01tmp/lex/phra-base-refs.xml \
	 >01tmp/lex/phra-base-remapped.xml

xsltproc $libscripts/lex-phra-base-HTML.xsl 01tmp/lex/phra-base-remapped.xml \
	 >01tmp/lex/lex-phra-base.xhtml

xsltproc $libscripts/lex-provides.xsl 01tmp/lex/lex-phra-base.xhtml \
	 >01tmp/lex/lex-provides-phra-base.xml

xsltproc -stringparam project `oraccopt` -stringparam type pb \
	 $libscripts/lex-split-HTML.xsl 01tmp/lex/lex-phra-base.xhtml
