#!/bin/sh
xmlfile=teiCorpus-`date --iso`.xml
xsltproc --xinclude \
    ${ORACC}/lib/scripts/trim-tei-project.xsl 02pub/tei-project.xml \
    >01tmp/$xmlfile
echo -n $xmlfile
