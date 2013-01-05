#!/bin/sh
./nm2xml.plx $1.txt >$1.xml
xsltproc --stringparam step-type oracci --xinclude nm-FO.xsl $1.xml >$1-oracci.fo
~/software/psdfop.sh $1-oracci.fo $1-oracci.pdf
open $1-oracci.pdf
