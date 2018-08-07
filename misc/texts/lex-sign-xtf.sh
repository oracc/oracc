#!/bin/sh
out=01tmp/lex/sign/`basename $1 .xtf`.lex
#echo lex-sign-data.xsl $1 into $out
xsltproc -o $out $ORACC_BUILDS/lib/scripts/lex-sign-data.xsl $1
