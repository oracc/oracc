#!/bin/sh
./nm2xml.plx $1.txt >$1.xml
xsltproc --stringparam step-type psd --xinclude nm-FO.xsl $1.xml >$1.fo
~/software/psdfop.sh $1.fo $1.pdf
open $1.pdf
