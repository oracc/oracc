#!/bin/sh
est_entry()
{
    xsltproc \
    -stringparam oracc $ORACC \
    -stringparam project `oraccopt` \
    -stringparam lang $1 \
    $ORACC/lib/scripts/est-articles.xsl 01bld/$1/articles.xml
    xl 01bld/$1/articles.xml | grep '<entry' | cut -d'"' -f2,4 >02pub/$1-urn-hack.txt
}

est_gloss()
{
    xsltproc \
    -stringparam oracc $ORACC \
    -stringparam project `oraccopt` \
    -stringparam lang $1 \
    $ORACC/lib/scripts/est-glossary.xsl 01bld/$1/articles.xml \
	>01bld/est/$l/glossary.est
}

for l in `ls 02www/cbd|grep -v ^qpn-x`; do
    rm -fr 01bld/est/$l
    mkdir -p 01bld/est/$l
    est_gloss $l
    est_entry $l
done
sort -u 02pub/*-urn-hack.txt >02pub/urn-hack.txt
chmod o+r 02pub/urn-hack.txt
