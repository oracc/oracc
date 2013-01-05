#!/bin/sh
for a in base-60 length surface volume drycap liqcap weight brick; \
    do echo $a && ./nm2xml.plx $a.txt >$a.xml; \
done
xsltproc --stringparam step-type psd --xinclude nm-FO.xsl nm.xml >nm.fo
xsltproc --stringparam step-type oracci --xinclude nm-FO.xsl nm.xml >nm-oracci.fo
~/software/psdfop.sh nm.fo nm.pdf
~/software/psdfop.sh nm-oracci.fo nm-oracci.pdf
open nm-oracci.pdf
