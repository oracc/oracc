#!/bin/sh
./nm2xml.plx length.txt >length.xml
xsltproc nm-system-FO.xsl length.xml >length.fo
~/software/psdfop.sh length.fo length.pdf
open length.pdf
