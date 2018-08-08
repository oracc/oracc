#!/bin/sh
out=01tmp/lex/word/`basename $1 .xtf`.lex
#echo lex-word-data.xsl $1 into $out
xsltproc -o $out $ORACC_BUILDS/lib/scripts/lex-word-data.xsl $1
