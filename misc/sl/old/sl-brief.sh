#!/bin/sh
project=`oraccopt`
title=`oraccopt . logo`
xsltproc -stringparam project $project -stringparam title $title \
	 $ORACC_BUILDS/lib/scripts/sl-summaries-HTML.xsl 02xml/sl-grouped.xml
