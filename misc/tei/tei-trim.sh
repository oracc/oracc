#!/bin/sh
xmlfile=teiCorpus-`date +%Y-%m-%d`.xml
xsltproc --xinclude \
    ${ORACC}/lib/scripts/trim-tei-project.xsl 02pub/tei-project.xml \
    >01tmp/$xmlfile
/bin/echo -n $xmlfile
