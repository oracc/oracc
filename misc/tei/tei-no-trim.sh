#!/bin/sh
proj=`oraccopt | tr / - `
xmlfile=$proj-teiCorpus-`date +%Y%m%d`.xml
xmllint --xinclude 02pub/tei-project.xml >01tmp/$xmlfile
/bin/echo -n $xmlfile
