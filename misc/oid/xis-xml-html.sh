#!/bin/sh
# add infile and -o outfile
proj=$1
xsltproc -stringparam divwrap yes \
	 -stringparam fragment yes \
	 -stringparam item-url yes \
	 -stringparam project $proj \
	 $ORACC_BUILDS/lib/scripts/p3-ce-HTML.xsl -
