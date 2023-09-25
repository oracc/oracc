#!/bin/sh
xtf=/home/oracc/ur3sp/nippur/01bld/P142/P142561/P142561.xtf
xsltproc --param standalone 'true()' --stringparam project ur3sp/nippur \
	 --param proofing-mode 'true()' \
	 $ORACC/lib/scripts/g2-xtf-HTML.xsl $xtf
