#!/bin/sh
xsltproc --stringparam project `oraccopt` \
    $xsl/xix-HTML.xsl $1 > $webdir/cbd/$lang/`basename $1 xix`html
