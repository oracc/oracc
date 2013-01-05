#!/bin/sh
./nm2xml.plx base-60.txt >base-60.xml
xsltproc nm-system-FO.xsl base-60.xml >base-60.fo
~/software/psdfop.sh base-60.fo base-60.pdf
open base-60.pdf
