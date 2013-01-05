#!/bin/sh
xmlfile=tei-`date --iso`.xml
xmllint --xinclude 02pub/tei-project.xml >01tmp/$xmlfile
echo -n $xmlfile
